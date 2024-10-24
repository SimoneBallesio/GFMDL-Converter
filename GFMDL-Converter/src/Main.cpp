#include "Pch.hpp"
#include "Version.hpp"

#include "Parsers/GfmdlParser.hpp"

#include "Output/Converter.hpp"

enum CommandLineArg
{
	None = 0,
	ConvertPath,
	OutputPath,
};

struct State
{
	std::string PathIn = "";
	OutputFormat OutFormat = OutputFormat::Unsupported;
	std::string PathOut = "";
	bool VerboseTrace = false;
};

static bool ParseCommandLineArgs(int argc, char** argv, State& state);
static void DisplayHelpMessage();
static bool IsStateValid(const State& state);

int main(int argc, char* argv[])
{
	State state = {};

	if (!ParseCommandLineArgs(argc, argv, state)) return 0;

	if (!IsStateValid(state))
	{
		std::cerr << "ERROR: Invalid paths or output format specified for file conversion, see --help for usage.\n";
		return 1;
	}

	GFMDLConv::GFMDLParser parser;

	if (!parser.Parse(std::move(state.PathIn.c_str())))
	{
		std::cerr << "\nERROR: Unable to parse source file.\n";
		return 1;
	}

	GFMDLConv::Converter conv(parser.GetMeshes(), state.OutFormat, state.PathOut);

	if (!conv.IsOK())
	{
		std::cerr << "\nERROR: The conversion process failed (see above for details).\n";
		return 1;
	}

	return 0;
}

static bool ParseCommandLineArgs(int argc, char** argv, State& state)
{
	if (argc < 2)
	{
		DisplayHelpMessage();
		return false;
	}

	CommandLineArg cmd = CommandLineArg::None;
	const std::vector<std::string_view> args(argv + 1, argv + argc);

	for (const auto& arg : args)
	{
		switch (cmd)
		{
			case CommandLineArg::None:
			{
				if (arg == "--help" || arg == "-h")
				{
					DisplayHelpMessage();
					return false;
				}

				else if (arg == "--version" || arg == "-v")
				{
					std::cout << GFMDL_CONV_VERSION_MAJOR << '.' << GFMDL_CONV_VERSION_MINOR << '.' << GFMDL_CONV_VERSION_PATCH << '\n';
					return 0;
				}

				else if (arg == "--verbose")
				{
					state.VerboseTrace = true;
					continue;
				}

				else if (arg == "-c")
				{
					cmd = CommandLineArg::ConvertPath;
					continue;
				}

				else if (arg == "-o")
				{
					cmd = CommandLineArg::OutputPath;
					continue;
				}

				std::cerr << "WARNING: Skipping unknown argument \"" << arg << "\"\n";
				continue;
			}
			break;

			case CommandLineArg::ConvertPath:
			{
				cmd = CommandLineArg::None;

				if (state.PathIn.size() > 0)
				{
					std::cerr << "ERROR: The source file's path has already been set (" << state.PathIn << "), skipping duplicate argument.\n";
					continue;
				}

				state.PathIn = arg;
				continue;
			}
			break;

			case CommandLineArg::OutputPath:
			{
				cmd = CommandLineArg::None;

				if (state.PathOut.size() > 0)
				{
					std::cerr << "ERROR: The output file's path has already been set (" << state.PathOut << "), skipping duplicate argument.\n";
					continue;
				}

				std::filesystem::path p(arg);

				if (strcmp(p.extension().c_str(), ".obj") == 0)
					state.OutFormat = OutputFormat::WavefrontObj;

				state.PathOut = arg;
				continue;
			}
			break;
		}
	}

	bool valid = state.PathIn.size() > 0;

#ifndef GFMDL_CONV_SKIP_OUTPUT

	valid &= state.PathOut.size() > 0;

#endif

	if (!valid)
	{
		std::cerr << "ERROR: Missing path arguments for conversion, see --help for usage.\n";
		return false;
	}

	return true;
}

static void DisplayHelpMessage()
{
	std::cout << "\n-- GFMDL-Converter --\n\n"
	          << "A tool to convert .gfmdl models into more standard 3D file formats.\n\n"
	          << "--help, -h                   Display this message\n"
	          << "--version, -v                Display the program's version\n"
	          << "-c <path/to/source.gfmdl>    Specify the source file path\n"
	          << "-o <path/to/output.ext>      Specify the output file path\n"
	          << "                             Formats supported: OBJ\n\n";
}

static bool IsStateValid(const State& state)
{
	bool valid = std::filesystem::exists(state.PathIn);

#ifndef GFMDL_CONV_SKIP_OUTPUT

	valid &= (state.PathOut.size() > 0 && state.OutFormat != OutputFormat::Unsupported);

	if (valid && std::filesystem::exists(state.PathOut))
	{
		std::cerr << "\nWARNING: The output file specified already exists, it will be overwritten.\n";
	}

#endif

	return valid;
}