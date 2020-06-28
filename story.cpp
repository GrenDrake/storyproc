#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include "story.h"


int main(int argc, char *argv[]) {
    bool quietMode = false;
    bool doOutput = false;
    std::string filename(""), outFilename("");
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            std::string arg(argv[i]);
            if ("-quiet" == arg) {
                quietMode = true;
            } else if ("-output" == arg) {
                doOutput = true;
            } else if ("-o" == arg) {
                doOutput = true;
                ++i;
                if (i >= argc) {
                    std::cerr << "-o expects filename.\n";
                    return 1;
                }
                outFilename = argv[i];
            } else {
                std::cerr << "Unrecgonized option " << arg << ".\n";
                return 1;
            }
        } else {
            if (filename.empty()) filename = argv[i];
            else {
                std::cerr << "Only one file may be specified.\n";
                return 1;
            }
        }
    }

    if (filename.empty()) {
        std::cerr << "No story file specified!\n";
        return 1;
    }
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open story file " << filename << ".\n";
        return 1;
    }
    // deal with pointless byte order marks
    if (file.peek() == 0xEF) {
        char bytes[3];
        file.read(bytes, 3);
        if (bytes[0] != static_cast<char>(0xEF) || bytes[1] != static_cast<char>(0xBB) || bytes[2] != static_cast<char>(0xBF)) {
            // not actually a byte order mark, so reset the file position
            file.seekg(0);
        }
    }

    std::stringstream body;
    std::string line;
    std::vector<std::string> text;
    Story story;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            body << "\n\n";
        } else if (line[0] == '%') {
            // comment line: skip
        } else if (line[0] == '/') {
            auto pos = line.find_first_of(" \t");
            if (pos != std::string::npos) {
                std::string command = trim(line.substr(0, pos));
                std::string argument = trim(line.substr(pos));

                if (command == "/chapter") {
                    story.addChapter(argument);
                    body << "## " << argument;
                } else if (command == "/scene") {
                    story.addScene(argument);
                    body << "\\* \\* \\*\n\n";
                } else if (command == "/title") {
                    story.title = argument;
                } else if (command == "/author") {
                    story.author = argument;
                } else if (command == "/date") {
                    story.date = argument;
                } else {
                    std::cerr << "Unhandled command " << command << ".\n";
                }
            } else {
                std::cerr << "Command " << line << " lacks argument.\n";
            }

            std::string block = body.str();
            block = trim(block);
            if (!block.empty()) {
                text.push_back(block);
            }
            body.str("");
            body.clear();
        } else {
            body << line << ' ';
            int words = countWords(line);
            story.addWordCount(words);
        }
    }
    text.push_back(body.str());

    if (!quietMode) {
        std::cout << std::left;
        for (const Chapter &chapter : story.chapters) {
            if (chapter.wordCount <= 0) continue;
            std::cout << std::setw(56) << chapter.name << "  " << chapter.wordCount << '\n';
            for (const Scene &scene : chapter.scenes) {
                if (scene.wordCount <= 0) continue;
                std::cout << "    " << std::setw(56) << scene.name << "  " << scene.wordCount << '\n';
            }
        }

        int rounded = story.wordCount / 100 * 100;
        if (story.wordCount % 100 > 50) rounded += 100;
        std::cout << "                                                          ------------\n";
        std::cout << "Total count                                               " << story.wordCount << '\n';
        std::cout << "Total count (rounded)                                     " << rounded << "\n\n";
    }

    if (doOutput) {
        std::string outputFile;
        if (outFilename.empty()) {
            outputFile = filename;
            auto pos = outputFile.find_last_of('.');
            if (pos != std::string::npos) {
                outputFile.replace(pos, std::string::npos, ".md");
            } else {
                outputFile += ".md";
            }
        } else {
            outputFile = outFilename;
        }

        std::ofstream outfile(outputFile);
        outfile << "# " << story.title << "\n\n_" << story.author;
        if (!story.date.empty()) outfile << " (" << story.date << ')';
        outfile << "_\n\n";
        for (const std::string &s : text) {
            std::string final = trim(s);
            if (!s.empty()) outfile << final << "\n\n";
        }
    }

    return 0;
}
