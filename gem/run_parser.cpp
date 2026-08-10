/*
 * Copyright (c) 2022, Michael Minnick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#include "run_parser.h"
#include "param_parser.h"
#include "parser.h"
#include "MidiOut.h"
#include "Gesture.h"
#include "generalmidi.h"
#include "Scheduler.h"
#include "WinUtil.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stack>

// This piece is modified by run parser commands and copied to the active piece by the start command.
Piece g_piece;

// Voice numbers run 1 through kNumVoices. Voice 10 is set to the percussion channel (MIDI channel 10, encoded as 9).
static constexpr size_t kNumVoices = 16;
static constexpr size_t kNumParamBlocks = 20;
static constexpr size_t kPercussionVoice = 10;

// The load command saves stdin here.
std::stack<std::streambuf*> g_streambuf_stack;
std::stack<std::ifstream> g_input_file_stack;
std::filesystem::path g_gemDirectory;

//FIXME remove
#if 0
static int paramLookup(const std::string& param_name)
{
	if (param_name == "rhythm") {
		return kRhythmIndex;
	}
	else if (param_name == "pitch") {
		return kPitchIndex;
	}
	else if (param_name == "velocity") {
		return kVelocityIndex;
	}
	else if (param_name == "instrument") {
		return kInstrumentIndex;
	}
	else if (param_name == "chord") {
		return kChordIndex;
	}
	else {
		return -1;
	}
}
#endif

// Temporary
static int tryGetInt(const std::string& str, int default_value)
{
	try {
		return std::stoi(str);
	}
	catch (const std::invalid_argument&) {
		std::cerr << "Invalid integer: " << str << "\n";
		return default_value;
	}
	catch (const std::out_of_range&) {
		std::cerr << "Integer out of range: " << str << "\n";
		return default_value;
	}
}
#if 0
static float tryGetFloat(const std::string& str, float default_value)
{
	try {
		return std::stof(str);
	}
	catch (const std::invalid_argument&) {
		std::cerr << "Invalid float: " << str << "\n";
		return default_value;
	}
	catch (const std::out_of_range&) {
		std::cerr << "Float out of range: " << str << "\n";
		return default_value;
	}
}

static std::vector<int> parseParamValues(const std::string& param_values_str)
{
	std::vector<int> values;
	std::istringstream iss(param_values_str);
	std::string token;
	while (std::getline(iss, token, ',')) {
		values.push_back(tryGetInt(token, 0));
	}
	return values;
}
#endif

static ParamBlock makeDefaultParamBlock()
{
    Gesture rhythm = make_gesture(-1000);
    Gesture pitch = make_gesture(c4);
    Gesture velocity = make_gesture(120);
    Gesture instrument = make_gesture(1);
    Gesture chord = make_gesture(1);
    const int pb_total_time = 0;
    ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch, velocity, instrument, chord);
    return pb;
}

static Voice makeDefaultVoice()
{
	Voice v;
    v.AddParamBlock(makeDefaultParamBlock());
	return v;
}

#if 0
static CommandParser::Result cmdSet(const std::vector<std::string>& args)
{
	if (args.size() < 4) {
		std::cerr << "Usage: set <voice#> <param_block#> <param_name> <param_value,>...\n";
		return CommandParser::Result::Continue;
	}
	const int voice_num = tryGetInt(args[0], 0);
    const int pb_num = tryGetInt(args[1], 0);
	const std::string& param_name = args[2];
	const std::string& param_value = args[3];

	if (voice_num >= g_piece.size()) {
		std::cerr << "Invalid voice number: " << voice_num << "\n";
		return CommandParser::Result::Continue;
	}
	if (pb_num >= g_piece[voice_num].m_param_blocks.size()) {
		std::cerr << "Invalid param block number: " << pb_num << "\n";
		return CommandParser::Result::Continue;
	}
	int param_index = paramLookup(param_name);
	if (param_index == -1) {
		std::cerr << "Unknown parameter name: " << param_name << "\n";
		return CommandParser::Result::Continue;
	}
	Gesture gesture{ parseParamValues(param_value) };
	//gesture.Print();

	g_piece[voice_num].m_param_blocks[pb_num].SetGesture(param_index, gesture);

	return CommandParser::Result::MaybeStart;
}
#endif

static CommandParser::Result cmdSet(const std::vector<std::string>& args)
{
	int argc = static_cast<int>(args.size());
	if (argc < 2) {
		std::cerr << "Usage: set <parameter> [options]\n";
		return CommandParser::Result::Continue;
	}
	std::string cmd = args[1];
	if (cmd == "autosubmit") {
		bool enabled;
		if (!ParseSetAutosubmit(argc, args, enabled)) {
			return CommandParser::Result::Continue;
		}
		//g_parser.SetAutoSubmit(enabled);
	}
	else if (cmd == "mute") {
		int voice;
		bool mute;
		if (!ParseSetMute(argc, args, voice, mute)) {
			return CommandParser::Result::Continue;
		}
		if (voice >= g_piece.size()) {
			std::cerr << "Invalid voice number: " << voice << "\n";
			return CommandParser::Result::Continue;
		}
		g_piece[voice].SetIsMuted(mute);
	}
	else if (cmd == "duration") {
		int voice, block;
		float seconds;
		bool loop;
		if (!ParseSetDuration(argc, args, voice, block, seconds, loop)) {
			return CommandParser::Result::Continue;
		}
		if (voice >= g_piece.size()) {
			std::cerr << "Invalid voice number: " << voice << "\n";
			return CommandParser::Result::Continue;
		}
		if (block >= g_piece[voice].m_param_blocks.size()) {
			std::cerr << "Invalid param block number: " << block << "\n";
			return CommandParser::Result::Continue;
		}
		int duration_ms = loop ? 0 : static_cast<int>(seconds * 1000.0f);
		g_piece[voice].m_param_blocks[block].SetDuration(duration_ms);
	}
	else if (cmd == "channel") {
		int voice, channel;
		if (!ParseSetChannel(argc, args, voice, channel)) {
			return CommandParser::Result::Continue;
		}
		if (voice >= g_piece.size()) {
			std::cerr << "Invalid voice number: " << voice << "\n";
			return CommandParser::Result::Continue;
		}
		g_piece[voice].SetChannelNumber(channel);
	}
	else if (cmd == "rhythm") {
		int voice, block;
		std::vector<int> values;
        if (!ParseSetRhythm(argc, args, voice, block, values)) {
			return CommandParser::Result::Continue;
		}
		if (voice >= g_piece.size()) {
			std::cerr << "Invalid voice number: " << voice << "\n";
			return CommandParser::Result::Continue;
		}
		if (block >= g_piece[voice].m_param_blocks.size()) {
			std::cerr << "Invalid param block number: " << block << "\n";
			return CommandParser::Result::Continue;
		}
		Gesture gesture{ values };
		g_piece[voice].m_param_blocks[block].SetGesture(kRhythmIndex, gesture);
	}

	return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdShow(const std::vector<std::string>& args)
{
	if (args.size() != 1) {
		std::cerr << "Usage: show\n";
		return CommandParser::Result::Continue;
	}

	// FIXME show autosubmit on or off

    size_t voice_num = 0;
	for (auto& voice : g_piece) {
		std::cout << "voice " << voice_num << ":\n";
		std::cout << "  channel: " << voice.GetChannelNumber() << "\n";
		std::cout << "  mute: " << voice.IsMuted() << "\n";
		size_t pb_num = 0;
		for (auto& pb : voice.m_param_blocks) {
			std::cout << "  param block: " << pb_num << "\n";
			int dur_ms = pb.GetDuration();
			float dur = static_cast<float>(dur_ms) / 1000.0f; // Convert milliseconds to seconds
			if (dur_ms > 0) {
				std::cout << "    duration: " << dur << " secs\n";
			}
			else {
				std::cout << "    duration: " << "0 secs (looped)\n";
			}
			std::cout << "    rhythm: ";
			pb.GetRhythmGesture().Print();
			std::cout << "    pitch: ";
			pb.GetPitchGesture().Print();
			std::cout << "    velocity: ";
			pb.GetVelocityGesture().Print();
			std::cout << "    instrument: ";
			pb.GetInstrumentGesture().Print();
			std::cout << "    chord: ";
			pb.GetChordGesture().Print();
			++pb_num;
		}
		++voice_num;
	}

	return CommandParser::Result::Continue;
}

static CommandParser::Result cmdMute(const std::vector<std::string>& args)
{
	if (args.size() < 2) {
		std::cerr << "Usage: mute <voice#> {1 | 0}\n";
		return CommandParser::Result::Continue;
	}
	const int voice_num = tryGetInt(args[0], 0);
	if (voice_num >= g_piece.size()) {
		std::cerr << "Invalid voice number: " << voice_num << "\n";
		return CommandParser::Result::Continue;
	}
	// move mute to voice. default mute is current mute. update voice thread to check
	const bool default_mute = g_piece[voice_num].IsMuted();
	const int new_mute = tryGetInt(args[1], default_mute);
	g_piece[voice_num].SetIsMuted(new_mute);

	return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdChannel(const std::vector<std::string>& args)
{
	if (args.size() < 2) {
		std::cerr << "Usage: channel <voice#> <chan#>\n";
		return CommandParser::Result::Continue;
	}
	const int voice_num = tryGetInt(args[0], 0);
	if (voice_num >= g_piece.size()) {
		std::cerr << "Invalid voice number: " << voice_num << "\n";
		return CommandParser::Result::Continue;
	}
	const int channel_num = tryGetInt(args[1], 0);
	g_piece[voice_num].SetChannelNumber(channel_num);

	return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdDuration(const std::vector<std::string>& args)
{
#if 0
	if (args.size() < 3) {
		std::cerr << "Usage: duration <voice#> <param_block#> {<duration_float_sec> | 0}\n";
		return CommandParser::Result::Continue;
	}
	const int voice_num = tryGetInt(args[0], 0);
	if (voice_num >= g_piece.size()) {
		std::cerr << "Invalid voice number: " << voice_num << "\n";
		return CommandParser::Result::Continue;
	}
	const int pb_num = tryGetInt(args[1], 0);
	if (pb_num >= g_piece[voice_num].m_param_blocks.size()) {
		std::cerr << "Invalid param block number: " << pb_num << "\n";
		return CommandParser::Result::Continue;
	}
	const float duration = tryGetFloat(args[2], 0.0f);
	if (duration < 0.0f) {
		std::cerr << "Invalid duration: " << duration << "\n";
		return CommandParser::Result::Continue;
	}
    const int duration_ms = static_cast<int>(duration * 1000.0f); // Convert seconds to milliseconds
    g_piece[voice_num].m_param_blocks[pb_num].SetDuration(duration_ms);
#endif
	return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdClear(const std::vector<std::string>& args)
{
	if (args.size() != 2 || args[1] != "all") {
		std::cerr << "Usage: clear all\n";
		return CommandParser::Result::Continue;
	}
	std::cout << "Do you really want to clear the whole piece? (y/n): ";
	std::string response;
	std::getline(std::cin, response);
	if (response.empty() ||
		(response[0] != 'y' && response[0] != 'Y')) {
		std::cout << "Piece not cleared.\n";
		return CommandParser::Result::Continue;
	}
	Piece p;
	g_piece = p;

	return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdLoad(const std::vector<std::string>& args)
{
	namespace fs = std::filesystem;

	if (args.size() != 2) {
		std::cerr << "Usage: load <file>\n";
		return CommandParser::Result::Continue;
	}
	std::string file_name = args[0];
    fs::path full_path = g_gemDirectory / file_name;
	std::cout << "Loading file " << full_path << "\n";
	g_input_file_stack.push(std::ifstream(full_path));
	if (!g_input_file_stack.top()) {
		std::cerr << "Unable to open file " << full_path << "\n";
		g_input_file_stack.pop();
		return CommandParser::Result::Continue;
	}

	// Save the original stream buffer.
	g_streambuf_stack.push(std::cin.rdbuf());

	// Redirect std::cin to the file.
	std::cin.rdbuf(g_input_file_stack.top().rdbuf());

	return CommandParser::Result::RedirectInput;
}

static CommandParser::Result cmdSave(const std::vector<std::string>& args)
{
	namespace fs = std::filesystem;

	if (args.size() != 2) {
		std::cerr << "Usage: save <file>\n";
		return CommandParser::Result::Continue;
	}
	std::string file_name = args[0];
    fs::path full_path = g_gemDirectory / file_name;
	std::cout << "Saving to file " << full_path << "\n";

	// Check whether the file already exists.
	if (fs::exists(full_path)) {
		std::cout << "'" << full_path
			<< "' already exists. Overwrite? (y/n): ";
		std::string response;
		std::getline(std::cin, response);
		if (response.empty() ||
			(response[0] != 'y' && response[0] != 'Y')) {
			std::cout << "File not written.\n";
			return CommandParser::Result::Continue;
		}
	}

	// Open for writing (truncates existing file).
	std::ofstream out(full_path);

	if (!out) {
		std::cerr << "Error: Unable to create file '"
			<< full_path << "'.\n";
		return CommandParser::Result::Continue;
	}

	size_t voice_num = 0;
	for (auto& voice : g_piece) {
		out << "# Voice " << voice_num << "\n";
        out << "add voice\n";
		out << "channel " << voice_num << " " << voice.GetChannelNumber() << "\n";
		out << "mute " << voice_num << " " << (voice.IsMuted() ? "1" : "0") << "\n";
		size_t pb_num = 0;
		for (auto& pb : voice.m_param_blocks) {
            out << "# Param Block " << pb_num << "\n";
			out << "add param_block " << voice_num << "\n";
			int dur_ms = pb.GetDuration();
			if (dur_ms > 0) {
				float dur = static_cast<float>(dur_ms) / 1000.0f; // Convert milliseconds to seconds
				out << "duration " << voice_num << " " << pb_num << " " << dur << " secs\n";
			}
			out << "set " << voice_num << " " << pb_num << " rhythm " << pb.GetRhythmGesture().Serialize() << "\n";
			out << "set " << voice_num << " " << pb_num << " pitch " << pb.GetPitchGesture().Serialize() << "\n";
			out << "set " << voice_num << " " << pb_num << " velocity " << pb.GetVelocityGesture().Serialize() << "\n";
			out << "set " << voice_num << " " << pb_num << " instrument " << pb.GetInstrumentGesture().Serialize() << "\n";
			out << "set " << voice_num << " " << pb_num << " chord " << pb.GetChordGesture().Serialize() << "\n";
			++pb_num;
		}
		++voice_num;
	}

	std::cout << "Saved.\n";
	return CommandParser::Result::Continue;
}

static CommandParser::Result cmdAdd(const std::vector<std::string>& args)
{
	size_t argc = args.size();
    int voice_num = 0;
	if (argc == 2) {
		Voice v = makeDefaultVoice();
		g_piece.push_back(v);
		voice_num = g_piece.size() - 1;
		std::cout << "Added voice " << voice_num << "\n";
		std::cout << "Added param block " << v.m_param_blocks.size() - 1 << "\n";
	}
	else if (ParseAddBlock(static_cast<int>(argc), args, voice_num)) {
		if (voice_num >= g_piece.size()) {
			std::cerr << "Invalid voice number: " << voice_num << "\n";
			return CommandParser::Result::Continue;
		}
		g_piece[voice_num].AddParamBlock(makeDefaultParamBlock());
	}
	else {
		return CommandParser::Result::Continue;
	}
    return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdQuit(const std::vector<std::string>& args)
{
	if (args.size() != 1) {
		std::cerr << "Usage: quit\n";
		return CommandParser::Result::Continue;
	}
	return CommandParser::Result::Quit;
}

static CommandParser::Result cmdStop(const std::vector<std::string>& args)
{
	if (args.size() != 1) {
		std::cerr << "Usage: stop\n";
		return CommandParser::Result::Continue;
	}
	return CommandParser::Result::Stop;
}

static CommandParser::Result cmdStart(const std::vector<std::string>& args)
{
	if (args.size() != 1) {
		std::cerr << "Usage: start\n";
		return CommandParser::Result::Continue;
	}
	return CommandParser::Result::Start;
}

static CommandParser::Result cmdHelp(const std::vector<std::string>&)
{
	std::cout << "Available commands:\n";
	std::cout << "  help\n";
	std::cout << "  quit\n";
	std::cout << "  stop\n";
	std::cout << "  start\n";
	std::cout << "  show\n";
	std::cout << "  add voice\n";
	std::cout << "  add block voice=<n>\n";
	std::cout << "  set rhythm voice=<n> block=<n> value=<n, n, n...>\n";
	std::cout << "  set pitch voice=<n> block=<n> value=<n, n, n...>\n";
	std::cout << "  set velocity voice=<n> block=<n> value=<n, n, n...>\n";
	std::cout << "  set chord voice=<n> block=<n> value=<n, n, n...>\n";
	std::cout << "  set autosubmit {on | off}\n";
	std::cout << "  set mute voice=<n> {on | off}\n";
	std::cout << "  set duration voice=<n> block=<n> {seconds=<n.n> | loop}\n";
	std::cout << "  set channel voice=<n> channel=<n>\n";
	std::cout << "  load <file>\n";
	std::cout << "  save <file>\n";
	std::cout << "  clear all\n";

	return CommandParser::Result::Continue;
}

static void init_parser(CommandParser& parser)
{
	parser.registerCommand("help", cmdHelp);
	parser.registerCommand("quit", cmdQuit);
	parser.registerCommand("stop", cmdStop);
	parser.registerCommand("start", cmdStart);
	parser.registerCommand("show", cmdShow);
	parser.registerCommand("add", cmdAdd);
	parser.registerCommand("set", cmdSet);
	parser.registerCommand("load", cmdLoad);
	parser.registerCommand("save", cmdSave);
	parser.registerCommand("clear", cmdClear);
}

static void play_rt_piece(Scheduler& s, Piece& p)
{
	s.Play(p);
}

int run_parser()
{
    CommandParser parser;
	init_parser(parser);

	Piece p;
	Scheduler* s = new Scheduler;
	std::thread play_thread;
	bool is_running = false;

	CommandParser::Result result{ CommandParser::Result::Continue };
	std::cout << "Type \"help\" for help.\n";

	g_gemDirectory = getDefaultDirectory();
	std::cout << "Save and Load path: " << g_gemDirectory << "\n";
    std::cout << "autosubmit is " << (parser.GetAutoSubmit() ? "on" : "off") << "\n";

	auto stop_if_running = [](Scheduler* s, std::thread& play_thread, bool& is_running)
	{
		if (is_running) {
			s->SetStop();
			play_thread.join();
			is_running = false;
		}
	};

	while (true)
	{
		result = parser.run();
		if (result == CommandParser::Result::RedirectInput) {
			if (g_streambuf_stack.empty() || g_input_file_stack.empty()) {
				std::cerr << "Error: Stream buffer or input file stack is empty.\n";
				continue;
            }
			// Restore std::cin.
			std::cin.rdbuf(g_streambuf_stack.top());
			g_streambuf_stack.pop();
			g_input_file_stack.top().close();
            g_input_file_stack.pop();
			std::cout << "Redirected input finished.\n";
			continue;
        }
		if (result == CommandParser::Result::Quit) {
			stop_if_running(s, play_thread, is_running);
			break;
		}
		if (result == CommandParser::Result::Stop) {
			stop_if_running(s, play_thread, is_running);
			continue;
		}
		if (result == CommandParser::Result::Start) {
			stop_if_running(s, play_thread, is_running);
			if (g_piece.empty()) {
				std::cerr << "No voices in the piece. Add voices before starting.\n";
				continue;
			} else {
				// Copy modified piece to the piece to play and start the new piece.
				p = g_piece;
				delete s;
                s = new Scheduler();
				play_thread = std::thread(play_rt_piece, std::ref(*s), std::ref(p));
				is_running = true;
			}
		}
	}
	delete s;

    return 0;
}
