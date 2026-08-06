#include "run_parser.h"
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

static CommandParser::Result cmdShow(const std::vector<std::string>&)
{
    size_t voice_num = 0;
	for (auto& voice : g_piece) {
		std::cout << "voice " << voice_num << ":\n";
		std::cout << "  channel: " << voice.GetChannelNumber() << "\n";
		std::cout << "  mute: " << voice.IsMuted() << "\n";
		size_t pb_num = 0;
		for (auto& pb : voice.m_param_blocks) {
			std::cout << "  param block: " << pb_num << "\n";
			int dur_ms = pb.GetDuration();
			if (dur_ms > 0) {
				float dur = static_cast<float>(dur_ms) / 1000.0f; // Convert milliseconds to seconds
				std::cout << "    duration: " << dur << " secs\n";
			}
			else {
				std::cout << "    duration: " << "(looped)\n";
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

	return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdClear(const std::vector<std::string>&)
{
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

	if (args.size() < 1) {
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

	if (args.size() < 1) {
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

	//size_t voice_num = 0;
	//for (auto& voice : g_piece) {
	//	if (voice.m_is_active) {
 //           out << "# Voice " << voice_num << "\n";
	//		out << "channel " << voice_num << " " << voice.GetChannelNumber() << "\n";
	//		size_t pb_num = 0;
	//		for (auto& pb : voice.m_param_blocks) {
	//			if (pb.m_is_active) {
 //                   out << "# Section " << pb_num << "\n";
	//				out << "section " << pb_num << "\n";
	//				int dur_ms = pb.GetDuration();
	//				float dur = static_cast<float>(dur_ms) / 1000.0f; // Convert milliseconds to seconds
	//				out << "duration " << dur << "\n";
	//				out << "set " << voice_num << " rhythm " << pb.GetRhythmGesture().Serialize() << "\n";
	//				out << "set " << voice_num << " pitch " << pb.GetPitchGesture().Serialize() << "\n";
	//				out << "set " << voice_num << " velocity " << pb.GetVelocityGesture().Serialize() << "\n";
	//				out << "set " << voice_num << " instrument " << pb.GetInstrumentGesture().Serialize() << "\n";
	//				out << "set " << voice_num << " chord " << pb.GetChordGesture().Serialize() << "\n";
	//				std::string s{ pb.IsMuted() ? "1" : "0" };
	//				out << "mute " << voice_num << " " << s << "\n";
	//			}
 //               ++pb_num;
 //           }
	//	}
	//	++voice_num;
	//}

	std::cout << "Saved.\n";
	return CommandParser::Result::Continue;
}

static CommandParser::Result cmdAdd(const std::vector<std::string>& args)
{
	if (args.size() == 1 && args[0] == "voice") {
		Voice v = makeDefaultVoice();
		g_piece.push_back(v);
		size_t new_voice_num = g_piece.size() - 1;
		std::cout << "Added voice " << new_voice_num << "\n";
		std::cout << "Added param block " << v.m_param_blocks.size() - 1 << "\n";
	}
	else if (args.size() == 2 && args[0] == "param_block") {
		const int voice_num = tryGetInt(args[1], 0);
		if (voice_num >= g_piece.size()) {
			std::cerr << "Invalid voice number: " << voice_num << "\n";
			return CommandParser::Result::Continue;
		}
		g_piece[voice_num].AddParamBlock(makeDefaultParamBlock());
	}
	else {
		std::cerr << "Usage: add {voice | param_block <voice#>}\n";
		return CommandParser::Result::Continue;
	}

    return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdHelp(const std::vector<std::string>&)
{
    std::cout << "Available commands:\n";
    std::cout << "  help\n";
	std::cout << "  {quit | exit}\n";
    std::cout << "  add {voice | param_block <voice#>}\n";
	std::cout << "  set <voice#> <param_block#> <param_name> <param_value,>...\n";
	std::cout << "  autosubmit\n";
	std::cout << "  stop\n";
	std::cout << "  {start | submit | play}\n";
	std::cout << "  show\n";
	std::cout << "  mute <voice#> {1 | 0}\n";
	std::cout << "  load <file>\n";
	std::cout << "  save <file>\n";
	std::cout << "  duration <voice#> <param_block#> {<duration_float_sec> | 0}\n";
	std::cout << "  clear\n";
	std::cout << "  channel <voice#> <chan#>\n";

	return CommandParser::Result::Continue;
}

static CommandParser::Result cmdQuit(const std::vector<std::string>&)
{
	return CommandParser::Result::Quit;
}

static CommandParser::Result cmdStop(const std::vector<std::string>&)
{
	return CommandParser::Result::Stop;
}

static CommandParser::Result cmdStart(const std::vector<std::string>&)
{
	return CommandParser::Result::Start;
}

static void init_parser(CommandParser& parser)
{
	parser.registerCommand("add", cmdAdd);
	parser.registerCommand("set", cmdSet);
	parser.registerCommand("help", cmdHelp);
	parser.registerCommand("quit", cmdQuit);
    parser.registerCommand("exit", cmdQuit);		// alias for quit
	parser.registerCommand("stop", cmdStop);
	parser.registerCommand("start", cmdStart);
    parser.registerCommand("submit", cmdStart);		// alias for start
	parser.registerCommand("play", cmdStart);		// alias for start
	parser.registerCommand("show", cmdShow);
	parser.registerCommand("mute", cmdMute);
	parser.registerCommand("load", cmdLoad);
	parser.registerCommand("save", cmdSave);
	parser.registerCommand("duration", cmdDuration);
	parser.registerCommand("clear", cmdClear);
	parser.registerCommand("channel", cmdChannel);
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
