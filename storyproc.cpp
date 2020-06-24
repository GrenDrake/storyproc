#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

// #include "maddy/parser.h"

std::string trim(const std::string &inStr);

bool g_isletter(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool g_isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int countWords(const std::string &line) {
    bool inWord = false;
    int count = 0;
    for (char c : line) {
        if (g_isletter(c)) {
            if (!inWord) {
                inWord = true;
                ++count;
            }
        } else if (g_isspace(c)) {
            inWord = false;
        }
    }
    return count;
}

struct Scene {
    std::string name;
    int wordCount;
};

struct Chapter {
    Chapter(const std::string &name)
    : name(name), wordCount(0)
    {
        scenes.push_back(Scene{"Opening"});
    }
    void addScene(const std::string &name) {
        scenes.push_back(Scene{name});
    }
    void addWordCount(int count) {
        wordCount += count;
        scenes.back().wordCount += count;
    }

    std::string name;
    int wordCount;
    std::vector<Scene> scenes;
};

struct Story {
    Story()
    : title("Unnamed"), author("Anonymous"), wordCount(0)
    {
        addChapter("Preamble");
    }
    void addChapter(const std::string &name) {
        chapters.push_back(Chapter(name));
    }
    void addScene(const std::string &name) {
        chapters.back().scenes.push_back(Scene{name});
    }
    void addWordCount(int count) {
        wordCount += count;
        chapters.back().addWordCount(count);
    }

    std::string title, author, date;
    int wordCount;
    std::vector<Chapter> chapters;
};

int main(int argc, char *argv[]) {
//     std::vector<std::string> tests{
//         "Hello world!",
//         "This is *really* great",
//         "Good-bye!",
//         "Dr. Jekyll. ",
//         "\"Hello there,\" say Angela."
//     };

//     for (const std::string &s : tests) {
//         int count = countWords(s);
//         std::cout << s << ": " << count << "\n";
//     }

// }
// int test() {

    bool quietMode = false;
    bool doOutput = false;
    std::string filename("");
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            std::string arg(argv[i]);
            if ("-quiet" == arg) {
                quietMode = true;
            } else if ("-output" == arg) {
                doOutput = true;
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
            // std::cout << "----------------------------------------------------------------------\n";
            std::cout << std::setw(56) << chapter.name << "  " << chapter.wordCount << '\n';
            for (const Scene &scene : chapter.scenes) {
                if (scene.wordCount <= 0) continue;
                std::cout << "    " << std::setw(56) << scene.name << "  " << scene.wordCount << '\n';
            }
        }

        int rounded = story.wordCount / 100 * 100;
        if (story.wordCount % 100 > 50) rounded += 100;
        // std::cout << "----------------------------------------------------------------------\n";
        std::cout << "                                                          ------------\n";
        std::cout << "Total count                                               " << story.wordCount << '\n';
        std::cout << "Total count (rounded)                                     " << rounded << "\n\n";
    }

    if (doOutput) {
        std::string outputFile = filename;
        auto pos = outputFile.find_last_of('.');
        if (pos != std::string::npos) {
            outputFile.replace(pos, std::string::npos, ".md");
        } else {
            outputFile += ".md";
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
