#include "TargaImageManipulator.hpp"
#include "TargaImage.h"

#include <cstring>
#include <cstdio>
#include <string>

using namespace std;

static const char brushes[] = "Available brushes are:\n"
                              "\tc (circle)\n"
                              "\tl (angled line)\n"
                              "An angled line is the default.\n\n";


/*!
 * \brief checks if a given string has a given ending
 * \param caseSensitive If false, the check is done without regards to
 *                      case sensitivity
 * \returns true if fullString ends with ending
 *
 * Used to check for file extensions
 */
static bool hasEnding (const string& fullString, const string& ending,
                       bool caseSensitive)
{
	if (fullString.length() < ending.length())
		return false;

	if (caseSensitive) {
		return fullString.compare(fullString.length() - ending.length(),
		                          ending.length(), ending) == 0;
	}
	else {
		string full = fullString;
		string end = ending;

		// Convert to lower case
		for (unsigned int c = 0; c < full.length(); ++c)
			full[c] = tolower(full[c]);

		for (unsigned int c = 0; c < ending.length(); ++c)
			end[c] = tolower(end[c]);

		return full.compare(full.length() - end.length(),
		                    end.length(), end) == 0;
	}
}

int main(int argc, char** argv)
{
	printf("\n");
	// Validate number of arguments
	if (argc < 3 || argc > 5) {
		printf(
		    "Usage: paint [input TGA] [output TGA] [brush]"
		    " [starting brush radius]"
		    "\n\tThe brush radius is in pixels."
		    "\n\tSpecifying the brush and radius is optional.\n%s",
		    brushes);
		return 1;
	}

	// Validate brush radius
	int radius;
	if (argc == 5) {
		if (sscanf(argv[4], "%d", &radius) != 1
		        || radius < 2 || radius > 15) {
			fprintf(stderr, "Error: brush radius must be between 2 and 15.\n");
			return 1;
		}
	}
	else {
		radius = 10;
	}

	// Determine the brush
	TargaImageManipulator::BrushType brush;
	if (argc > 3) {
		char method = 0;
		if (strlen(argv[3]) != 1
		        || sscanf(argv[3], "%c", &method) != 1) {
			fprintf(stderr, "Error: Unrecognized brush.\n%s", brushes);
			return 1;
		}
		switch (method) {
		case 'c':
		case 'C':
			brush = TargaImageManipulator::BR_CIRCLE;
			break;

		case 'l':
		case 'L':
			brush = TargaImageManipulator::BR_LINE;
			break;

		default:
			fprintf(stderr, "Error: Unrecognized brush.\n%s", brushes);
			return 1;
		}
	}
	else {
		brush = TargaImageManipulator::BR_LINE;
	}

	// Validate input file
	if (!hasEnding(argv[1], ".tga", false)) {
		printf("Warning: Input file does not end in the standard TGA extension."
		       "\n\tAn attempt to read it will be made anyways.\n");
	}

	TargaImage* ti = TargaImage::readImage(argv[1]);
	if (ti == nullptr) {
		fprintf(stderr, "The input file could not be read.\n");
		return 1;
	}

	// Warn about bad output names
	if (!hasEnding(argv[2], ".tga", false)) {
		printf(
		    "Warning: Output file does not end in the standard TGA extension."
		    "\n\tThe file will be written anyways.\n");
	}

	// Paint
	TargaImageManipulator::paint(ti, brush, radius);

	if (ti->write(argv[2]) != 1) {
		fprintf(stderr, "The output file could not be written.\n");
		return 1;
	}

	// There's no point in deleting things here. Nothing is written out to disk
	// in the destructors, and the OS will reclaim all memory on program exit.
	return 0;
}
