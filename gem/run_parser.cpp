//#include <thread>

#include "run_parser.h"
#include "parser.h"
#include "MidiOut.h"
#include "Gesture.h"
#include "generalmidi.h"
#include "Scheduler.h"

Piece g_piece;

static CommandParser::Result cmdSet(const std::vector<std::string>& args)
{
	//if (args.size() < 2) {
	//	std::cerr << "Usage: set <name> <value>\n";
	//	return CommandParser::Result::Continue;
	//}

	//const std::string& name = args[0];
	//const std::string& value = args[1];
	(void)args;

	//if (name == "default-voice-count") {
	//	int count = std::stoi(value);
	//	std::cout << "Setting default voice count to " << count << "\n";
	//	// TODO: apply setting
	//	return 0;
	//}

	g_piece[0].m_param_blocks[0].SetGesture(1, make_gesture(fs5));

	//std::cerr << "Unknown setting: " << name << "\n";
	return CommandParser::Result::MaybeStart;
}

static CommandParser::Result cmdHelp(const std::vector<std::string>&)
{
    std::cout << "Available commands:\n";
    std::cout << "  help\n";
	std::cout << "  quit\n";
	std::cout << "  set <voice#> <param_name> <param_values>\n";
	std::cout << "  autosubmit\n";
	std::cout << "  submit\n";
	std::cout << "  stop\n";
	std::cout << "  start\n";
	std::cout << "  show\n";
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
