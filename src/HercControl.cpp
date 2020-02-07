#include <rang.hpp> // Needs to be before anything including windows.h ...

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <array>
#include <memory>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <regex>

#include <cpr/cpr.h>
#include <CLI/CLI.hpp>

#include <config.h>

using namespace std;

void callHerculesConsole(string command, string waitFor, vector<string>& console);

void callHerculesConsole(string command, int requested_console_size, vector<string>& console);
void getResponseFromMarker(string command, string marker, vector<string>& console);
string makeMarker();
string trim(string str);
bool foundEnd(string needle, string hayStack);

// Global Configuration
auto host = "127.0.0.1:8038"s; // where is the herculese web console
auto startHistorySize = 10;    // get 10 lines to start with, then double etc.
auto timeOut = 30;             // Give up waiting to the Waitfor string after XXs if the console has not been updated
auto sleepWait = 250;          // Wait 250 ms before getting console updates
auto maxConsoleSize = 20;      // When the console gets bigger than this, re-mark it
auto debug = false;
auto getmarker = false;
auto marker = ""s;
auto single = false;
auto quiet = false;

// Global Variables
auto currentHistorySize = startHistorySize;
vector<string> saveConsole;
vector<string> console;

void outputConsole()
{
	if (quiet) return;
	if (single)
	{
		if (!console.empty())
			cout << console.back() << endl;
		return;
	}
	cout << rang::fg::green;
	for (const auto i : console)
		cout << i << endl;
	cout << rang::style::reset;
}

int main(int argc, char** argv)
{
	auto showVersion = false;
	string command;
	string waitFor;
	
	CLI::App app{ "Tool to send commands to Hercules", "herccontrol" };
	try
	{
		app.add_option("command", command, "The command to send to Hercules"s);
		app.add_option("-w,--waitfor", waitFor, "The regex string we are waiting for"s);
		app.add_flag("-m,--mark", getmarker, "Set and output mark point (to be used in herccontrol -f)"s);
		app.add_option("-f,--frommark", marker, "Search log from mark point (returned from herccontrol -m)"s);
		app.add_flag("-d,--debug", debug, "Debug mode"s)->envname("HC_DEBUG");
		app.add_flag("-q,--quiet", quiet, "Quiet mode (no output)"s);
		app.add_flag("-s,--single", single, "Single mode (only output last line)"s);
		app.add_option("-u,--url", host, "Host URL - default is "s + host)->envname("HC_HOSTURL");
		app.add_flag("-v,--version", showVersion, "Show version and exit"s);
		app.add_option("-t,--timeout", timeOut, "Timeout/sec - default is "s + to_string(timeOut))->envname("HC_TIMEOUT");
		app.add_option(",--starthistorysize", startHistorySize, "Start History Size/lines (advanced) - default is "s + to_string(startHistorySize));
		app.add_option(",--pollingtime", sleepWait, "Polling Time/ms (advanced) - default is "s + to_string(sleepWait));
		app.add_option(",--maxconsolesize", maxConsoleSize, "Max Console Size/lines (advanced) - default is "s + to_string(maxConsoleSize));
		app.allow_windows_style_options(false);

		app.parse(argc, argv);

		if (debug)
		{
			cerr << rang::fg::cyan << "Version: " << PROJECT_VER << rang::style::reset << std::endl;
			cerr << rang::fg::cyan << "Command: " << command << rang::style::reset << endl;
			cerr << rang::fg::cyan << "Wait for: " << waitFor << rang::style::reset << endl;
			cerr << rang::fg::cyan << "Host: " << host << rang::style::reset << endl;
			cerr << rang::fg::cyan << "Timeout/sec: " << timeOut << rang::style::reset << endl;
			cerr << rang::fg::cyan << "Start History Size/lines: " << startHistorySize << rang::style::reset << endl;
			cerr << rang::fg::cyan << "Polling Time/ms: " << sleepWait << rang::style::reset << endl;
			cerr << rang::fg::cyan << "Max Console Size/lines: " << maxConsoleSize << rang::style::reset << endl;
		}

		if (showVersion)
		{
			cout << rang::fg::cyan << PROJECT_VER << rang::style::reset << std::endl;
			return 0;
		}

		if (getmarker)
		{
			if (command != ""s || waitFor != ""s)
				throw CLI::Error("ARG_ERROR", "-m cannot be used with a command or -w value", 2);
		
			marker = makeMarker();
			callHerculesConsole("* "s + marker, 0, console);
			cout << marker << std::endl;
			return 0;
		}

		callHerculesConsole(command, waitFor, console);
	}
	catch (const CLI::Error & e) 
	{
		if (e.get_exit_code()) {
			cerr << rang::fg::red << rang::style::bold;
			app.exit(e);
			cerr << rang::style::reset;
			return 2;
		}
		cout << rang::fg::cyan;
		app.exit(e);
		cout << rang::style::reset;
		return 0;
	}
	catch (runtime_error & ex)
	{
		outputConsole();
		cerr << rang::style::bold << rang::fg::red << "ERROR: " << ex.what() << rang::style::reset << endl;
		return 1;
	}

	outputConsole();

	return 0;
}

int secondsSince(chrono::steady_clock::time_point start)
{
	auto end = chrono::steady_clock::now();
	return (int)chrono::duration_cast<chrono::seconds>(end - start).count();
}

void callHerculesConsole(string command, string waitFor, vector<string>& console)
{
	// Mark the console log so we can find the begining of our commands output
	auto newMarker = ""s;
	auto begin = chrono::steady_clock::now();

	if (waitFor.length() == 0)
	{
		callHerculesConsole(command, 0, console);
		return;
	}
	else
	{
		if (marker == ""s) {
			marker = makeMarker();
			callHerculesConsole("* "s + marker, 1, console);
		}

		getResponseFromMarker(command, marker, console);

		while (secondsSince(begin) < timeOut)
		{
			auto savedConsoleLength = console.size();

			// Find the string ...
			for (auto i = console.begin(); i != console.end(); i++)
			{
				if (foundEnd(*i, waitFor))
				{
					for (i++; i != console.end(); console.erase(i))
						; // erase the rest of the output

					 // Prepend the saved Console
					console.insert(console.begin(), saveConsole.begin(), saveConsole.end());

					return;
				}
			}

			// Not found - wait and refresh
			if (console.size() > maxConsoleSize)
			{
				// First the console is getting too big - need to re-mark the begining
				auto hasRemarked = false;

				// Put a new mark in the console log
				newMarker = makeMarker();
				callHerculesConsole("* "s + newMarker, 1, console);

				// Get the history since the old marker
				getResponseFromMarker(""s, marker, console);

				// Find the new marker
				for (auto i = console.begin(); i != console.end(); i++)
				{
					if (foundEnd(*i, waitFor))
					{
						// Wow we found what we were waiting for in the meantime!
						// erase the rest of the output
						for (i++; i != console.end(); console.erase(i))
							;

						// Prepend the saved Console
						console.insert(console.begin(), saveConsole.begin(), saveConsole.end());

						return;
					}
					else if (*i == "* "s + newMarker)
					{
						// Keep what was logged before
						// First ... Erase the rest of the output including the marker
						for (; i != console.end(); console.erase(i))
							;
						saveConsole.insert(saveConsole.end(), console.begin(), console.end());

						marker = newMarker;
						currentHistorySize = startHistorySize;
						console.clear();
						hasRemarked = true;
						savedConsoleLength = -1;
						if (debug)
							cerr << rang::fg::cyan << "Remark done" << rang::style::reset << endl;
						break;
					}
				}
				if (!hasRemarked)
					throw runtime_error("Failed to Remark output");
			}

			// Now we can wait and refresh
			this_thread::sleep_for(chrono::milliseconds(sleepWait));
			getResponseFromMarker("", marker, console);
			if (savedConsoleLength != console.size())
			{
				// Something has been writen to the console - so reset the timeout
				begin = chrono::steady_clock::now();
				if (debug)
					cerr << rang::fg::cyan << "Timeout Reset" << rang::style::reset << endl;
			}
		}
		throw runtime_error("Timeout");
	}
}

void getResponseFromMarker(string command, string marker, vector<string>& console)
{
	callHerculesConsole(command, currentHistorySize, console);

	auto consoleSize = console.size();
	static size_t lastConsoleSize = -1;

	if (lastConsoleSize != -1)
	{
		if (consoleSize == lastConsoleSize)
		{
			// We must have got right back to the begining!
			if (consoleSize == 0)
				throw runtime_error("Can't find marker - console size is zero");

			throw runtime_error("Can't find marker");
		}
	}

	for (auto i = console.begin(); i != console.end();)
	{
		if (*i == "* "s + marker)
		{
			console.erase(i);
			lastConsoleSize = -1;
			return;
		}
		else
			i = console.erase(i);
	}

	// Not found - we need more history!
	currentHistorySize *= 2;
	if (debug)
		cerr << rang::fg::cyan << "History extended to " << currentHistorySize << rang::style::reset << endl;

	// Recursive
	lastConsoleSize = consoleSize; // allows us to check is we have all history
	getResponseFromMarker(""s, marker, console);
}

void callHerculesConsole(string command, int requested_console_size, vector<string>& console)
{
	string result;
	auto keep = false;
	auto needToCheckFirstLine = true;
	int lines_wanted = requested_console_size?requested_console_size:1;
	auto begin = chrono::steady_clock::now();

	this_thread::sleep_for(chrono::milliseconds(sleepWait));

	console.clear();

	auto cmd = R"(http://)"s;
	cpr::Response http_call;
	cmd.append(host);
	cmd.append(R"(/cgi-bin/tasks/syslog)"s);
	if (debug)
		cerr << rang::fg::cyan << "Command: " << cmd << rang::style::reset << endl;
	while (secondsSince(begin) < timeOut) {

		http_call = cpr::Get(cpr::Url{ cmd }, cpr::Parameters{ {"command"s, command}, {"msgcount"s, to_string(lines_wanted)} });
		if (debug) {
			cerr << rang::fg::cyan << "Error Code: "s << (int)http_call.error.code << rang::style::reset << endl;
			cerr << rang::fg::cyan << "Error Message: "s << http_call.error.message << rang::style::reset << endl;
		}
		if (http_call.status_code < 200 || http_call.status_code > 299)
		{
			if (http_call.error.code == cpr::ErrorCode::CONNECTION_FAILURE) {
				this_thread::sleep_for(chrono::milliseconds(sleepWait));
			}
			else throw runtime_error(http_call.error.message + ". CPR-RC="s + to_string((int)http_call.error.code) + ". HTTP-RC="s + to_string(http_call.status_code));
		}
		else break; // Done!
	}
	if (http_call.status_code < 200 || http_call.status_code > 299)
	{
		throw runtime_error(http_call.error.message + ". CPR-RC="s + to_string((int)http_call.error.code) + ". HTTP-RC="s + to_string(http_call.status_code));
	}


	// Ignore any return
	if (requested_console_size == 0) return;

	if (http_call.text.length() == 0)
	{
		throw runtime_error("No response body");
	}

	stringstream response(http_call.text);
	string line;

	while (getline(response, line, '\n'))
	{
		line = trim(line);

		// Need to check that the first line is <html>
		if (needToCheckFirstLine)
		{
			if (line != "<html>"s)
			{
				throw runtime_error("Unexpected Data: "s + line);
			}
			needToCheckFirstLine = false;
		}

		if (keep)
		{
			if (line.compare(R"(</PRE>)"s) == 0)
				keep = false;
			else
				console.push_back(line);
		}
		else if (line.compare(R"(<PRE>)"s) == 0)
			keep = true;
	}
}

string makeMarker()
{
	using namespace std::chrono;

	static auto counter = 1;

	// get current time
	auto now = system_clock::now();

	// get number of milliseconds for the current second
	// (remainder after division into seconds)
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

	// convert to std::time_t in order to convert to std::tm (broken time)
	auto timer = system_clock::to_time_t(now);

	// convert to broken time
#ifdef _WIN32
	tm bt;
	localtime_s(&bt, &timer);
#else
	std::tm bt = *std::localtime(&timer);
#endif

	std::ostringstream oss;
	oss << std::put_time(&bt, "%Y-%m-%d_%H:%M:%S"); // HH:MM:SS
	oss << '.' << std::setfill('0') << std::setw(4) << ms.count();
	oss << "#" << std::setfill('0') << std::setw(8) << counter++;

	return oss.str();
}

string trim(string str)
{
	// Hercules sends bell \a - so lets get rid of everything weird
	auto first = str.find_first_not_of(" \n\r\t\a\b\f\v"s);
	if (string::npos == first)
		return str;

	auto last = str.find_last_not_of(" \n\r\t\a\b\f\v"s);
	return str.substr(first, (last - first + 1));
}

bool foundEnd(string needle, string hayStack)
{
	std::smatch match;
	std::regex regexSearch(hayStack);
	if (regex_search(needle, match, regexSearch))
		return true;
	return false;
}
