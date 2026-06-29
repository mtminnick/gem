//#include <thread>

#include "run_parser.h"
#include "parser.h"
#include "MidiOut.h"
#include "Gesture.h"
#include "generalmidi.h"
#include "Scheduler.h"

Piece g_piece;

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
		std::cerr << "Unknown parameter name: " << param_name << "\n";
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

static Voice make_default_voice()
{
	Voice v;
	Gesture rhythm = make_gesture(1000, -1000);
	Gesture pitch = make_gesture(c4);
	Gesture velocity = make_gesture(120);
	Gesture instrument = make_gesture(1);
	Gesture chord = make_gesture(1);
	const int pb_total_time = 0;
	ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch, velocity, instrument, chord);
	v.AddParamBlock(pb);
	return v;
}

static CommandParser::Result cmdAdd(const std::vector<std::string>& args)
{
	bool percussion_channel = false;
	if (args.size() == 1) {
		if (args[0] == "percussion") {
			percussion_channel = true;
		}
	}

	Voice v = make_default_voice();
	if (percussion_channel) {
		std::cout << "Adding percussion voice.\n";
		v.SetVoiceNumberOnce(kPercussionChannel);
	}
	else {
		std::cout << "Adding default voice.\n";
	}
	g_piece.push_back(v);

	return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdSet(const std::vector<std::string>& args)
{
	if (args.size() < 3) {
		std::cerr << "Usage: set <voice#> <param_name> <param_value,param_value...>\n";
		return CommandParser::Result::Continue;
	}

	const int voice_num = tryGetInt(args[0], 0);
	const std::string& param_name = args[1];
	const std::string& param_value = args[2];

	std::cout << voice_num << " " << param_name << " " << param_value << "\n";
	if (voice_num >= g_piece.size()) {
		std::cerr << "Invalid voice number: " << voice_num << "\n";
		return CommandParser::Result::Continue;
	}
	int param_index = paramLookup(param_name);
	if (param_index == -1) {
		return CommandParser::Result::Continue;
	}
	Gesture gesture{ parseParamValues(param_value) };
	gesture.Print();

	g_piece[voice_num].m_param_blocks[0].SetGesture(param_index, gesture);

	return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdShow(const std::vector<std::string>& args)
{
	if (args.size() < 1) {
		std::cerr << "Usage: show <voice#>\n";
		return CommandParser::Result::Continue;
	}
	const int voice_num = tryGetInt(args[0], 0);
	if (voice_num >= g_piece.size()) {
		std::cerr << "Invalid voice number: " << voice_num << "\n";
		return CommandParser::Result::Continue;
	}
	const Voice& voice = g_piece[voice_num];
	const ParamBlock& pb = voice.m_param_blocks[0];
	std::cout << "Voice " << voice_num << ":\n";
	std::cout << "  Rhythm: ";
	pb.GetRhythmGesture().Print();
	std::cout << "  Pitch: ";
	pb.GetPitchGesture().Print();
	std::cout << "  Velocity: ";
	pb.GetVelocityGesture().Print();
	std::cout << "  Instrument: ";
	pb.GetInstrumentGesture().Print();
	std::cout << "  Chord: ";
	pb.GetChordGesture().Print();
	return CommandParser::Result::Continue;
}

static CommandParser::Result cmdHelp(const std::vector<std::string>&)
{
    std::cout << "Available commands:\n";
    std::cout << "  help\n";
	std::cout << "  quit\n";
	std::cout << "  set <voice#> <param_name> <param_value,param_value...>\n";
	std::cout << "  autosubmit\n";
	std::cout << "  submit\n";
	std::cout << "  stop\n";
	std::cout << "  start\n";
	std::cout << "  show <voice#>\n";
	std::cout << "  add [percussion]\n";
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
	parser.registerCommand("set", cmdSet);
	parser.registerCommand("help", cmdHelp);
	parser.registerCommand("quit", cmdQuit);
	parser.registerCommand("exit", cmdQuit);
	parser.registerCommand("stop", cmdStop);
	parser.registerCommand("start", cmdStart);
	parser.registerCommand("submit", cmdStart);
	parser.registerCommand("show", cmdShow);
	parser.registerCommand("add", cmdAdd);
}

static Piece create_default_piece()
{
	// Default piece must contain every gesture.
	Gesture pitch = make_gesture(c4);
	Gesture rhythm = make_gesture(1000, -1000);
	Gesture velocity = make_gesture(120);

	Gesture instrument = make_gesture(1);
	Gesture chord = make_gesture(1);
	const int pb_total_time = 0;
	ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch, velocity, instrument, chord);
	Voice v = make_voice(pb);
	Piece p = make_piece(v);
	return p;
}

static void play_rt_piece(Scheduler& s, Piece& p)
{
	s.Play(p);
}

int run_parser()
{
    CommandParser parser;
	init_parser(parser);

	Piece p = create_default_piece();
	// Global modified piece starts as a copy of the default piece.
	g_piece = p;

	Scheduler* s = new Scheduler();
	std::thread play_thread{ play_rt_piece, std::ref(*s), std::ref(p) };
	bool is_running = true;

	CommandParser::Result result{ CommandParser::Result::Continue };
	std::cout << "Type \"help\" for help.\n";

	auto stop_if_running = [](Scheduler* s, std::thread& play_thread, bool& is_running)
	{
		if (is_running)
		{
			s->SetStop();
			play_thread.join();
			is_running = false;
		}
	};

	while (true)
	{
		result = parser.run();
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
			// Copy modified piece to the piece to play and start the new piece.
			p = g_piece;
			delete s;
			s = new Scheduler();
			play_thread = std::thread(play_rt_piece, std::ref(*s), std::ref(p));
			is_running = true;
		}
	}
	delete s;

    return 0;
}
