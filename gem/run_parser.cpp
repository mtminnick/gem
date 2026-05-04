//#include <thread>

#include "run_parser.h"
#include "parser.h"
#include "MidiOut.h"
#include "Gesture.h"
#include "generalmidi.h"
#include "Scheduler.h"

Piece g_piece;

static int cmdSet(const std::vector<std::string>& args)
{
	if (args.size() < 2) {
		std::cerr << "Usage: set <name> <value>\n";
		return 1;
	}

	const std::string& name = args[0];
	const std::string& value = args[1];

	if (name == "default-voice-count") {
		int count = std::stoi(value);
		std::cout << "Setting default voice count to " << count << "\n";
		// TODO: apply setting
		return 0;
	}

	std::cerr << "Unknown setting: " << name << "\n";
	return 1;
}

static int cmdHelp(const std::vector<std::string>&)
{
    std::cout << "Available commands:\n";
    std::cout << "  help\n";
	std::cout << "  quit\n";
	std::cout << "  set <name> <value>\n";
	std::cout << "  submit\n";
	std::cout << "  stop\n";
	std::cout << "  start\n";
	return 0;
}

static void init_parser(CommandParser& parser)
{
	parser.registerCommand("set", cmdSet);
	parser.registerCommand("help", cmdHelp);
}

static Piece create_default_piece()
{
	Gesture pitch = make_gesture(c4);
	Gesture rhythm = make_gesture(1000, -1000);
	const int pb_total_time = 0;
	ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch);
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

	while (true)
	{
		result = parser.run();
		if (result == CommandParser::Result::Quit) {
			// TODO make this a lambda
			if (is_running)
			{
				s->SetStop();
				play_thread.join();
				is_running = false;
			}
			break;
		}
		if (result == CommandParser::Result::Stop) {
			if (is_running)
			{
				s->SetStop();
				play_thread.join();
				is_running = false;
			}
		}
		if (result == CommandParser::Result::Start) {
			if (is_running)
			{
				s->SetStop();
				play_thread.join();
				is_running = false;
			}
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
