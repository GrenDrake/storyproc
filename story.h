#ifndef STORY_H
#define STORY_H

#include <string>
#include <vector>

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

std::string trim(const std::string &inStr);
bool g_isletter(int c);
bool g_isspace(int c);
int countWords(const std::string &line);


#endif
