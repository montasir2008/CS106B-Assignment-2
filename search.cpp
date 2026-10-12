// Name: Montasir Mahir & Weijia Li
// Course - CS 106B
// Section - Thu 7:30 pm
// Description - This file implements a simple search engine.
// The program cleans words and builds an index of the web pages containing them.
// Users can enter search terms to find matching pages.
// A '+' before a term keeps pages containing that term, while a '-' removes them.
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "SimpleTest.h" // IWYU pragma: keep (needed to quiet spurious warning)
using namespace std;


// This function takes a string s and returns a cleaned version of it.
// It changes all letters to lowercase and keeps only letters and digits.
// If s is empty or contains only punctuation, it returns an empty string.
string cleanToken(string s) {
    string temp;
    s = toLowerCase(s);
    for (int i = 0; i < s.length(); i++) {
        if (isalpha(s[i]) || isdigit(s[i])) {
            temp += s[i];
        }
    }
    return temp;
}

// This function takes a string text and returns a set of its unique words.
// It splits text at spaces and uses cleanToken to clean each word.
// Empty words are skipped, and repeated words are only added once.
Set<string> gatherTokens(string text) {
    Set<string> tokens;
    Vector<string> splitWords = stringSplit(text, ' ');
    for (string word : splitWords) {
        word = cleanToken(word);
        if (word.size() > 0){
            tokens.add(word);
        }
    }
    return tokens;
}

// This function takes a file name dbfile and a map index passed by reference.
// The file contains pairs of lines: a URL followed by the page's text.
// It cleans the words from each page and adds the URL to each word's set
// in index. It returns the number of web pages read.
// If the file cannot be opened, it raises an error.
int buildIndex(string dbfile, Map<string, Set<string>>& index) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    Vector<string> lines = stringSplit(readEntire(in), '\n');

    for (int i = 1; i < lines.size(); i += 2) {
        Set<string> tokens = gatherTokens(lines[i]);
        for (string word : tokens) {
            index[word] += lines[i-1];
        }
    }

    int webPages = lines.size()/2;

    return webPages;
}

// This function takes a map index and a string query and returns a set
// of matching URLs. It cleans each search term and looks it up in index.
// Terms are processed from left to right. Regular terms add matching
// pages, '+' terms keep shared pages, and '-' terms remove matching pages.
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query) {
    Set<string> result;
    Set<string> temp;
    Vector<string> components = stringSplit(query,' ');

    for (int i = 0; i < components.size(); i++) {
        char prefix = components[i][0];
        components[i] = cleanToken(components[i]);
        temp = index[components[i]];
        if (isalpha(prefix)) {
            result += temp;
        }
        else if (prefix == '+') {
            result.intersect(temp);
        }
        else if (prefix == '-') {
            result.difference(temp);
        }
    }
    return result;
}

// This function takes a file name dbfile and builds a search index from it.
// It prints the number of pages and unique terms, then asks the user
// to enter queries and prints the matching URLs.
// It stops when the user enters an empty line and does not return a value.
void searchEngine(string dbfile) {
    Map<string, Set<string>> index;
    int pages = buildIndex(dbfile,index);
    Vector<string> terms = index.keys();
    int numOfTerms = terms.size();

    cout << "Stand by while building index..." << endl;
    cout << "Indexed " << pages << " pages containing " << numOfTerms << " unique terms." << endl;
    cout << " " << endl;

    string input = "";
    while(true) {
        input = getLine("Enter query sentence (RETURN/ENTER to quit): ");
        if (input.empty()) {
            break;
        }
        Set<string> results = findQueryMatches(index, input);
        int numOfResults = results.size();
        cout << "Found " << numOfResults << " matching pages" << endl;
        cout << results << endl;
        cout << " " << endl;
    }

}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings of letters and digits") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS106B"), "cs106b");
}

PROVIDED_TEST("cleanToken on strings containing punctuation") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
    EXPECT_EQUAL(cleanToken("they're"), "theyre");
}

PROVIDED_TEST("cleanToken on string of only punctuation") {
    EXPECT_EQUAL(cleanToken("#$^@@.;"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 6 unique tokens, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 6);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 12 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 12);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}

STUDENT_TEST("cleanToken on an empty string") {
    EXPECT_EQUAL(cleanToken(""), "");
}

STUDENT_TEST("cleanToken on strings containing only digits") {
    EXPECT_EQUAL(cleanToken("123456"), "123456");
    EXPECT_EQUAL(cleanToken("007"), "007");
}

STUDENT_TEST("cleanToken on mixed letters, digits, and punctuation") {
    EXPECT_EQUAL(cleanToken("!CS-106B?"), "cs106b");
    EXPECT_EQUAL(cleanToken("A1.B2,C3"), "a1b2c3");
    EXPECT_EQUAL(cleanToken("...HeLLo!!!"), "hello");
}

STUDENT_TEST("gatherTokens from empty text") {
    Set<string> expected;
    EXPECT_EQUAL(gatherTokens(""), expected);
}

STUDENT_TEST("gatherTokens ignores punctuation-only words") {
    Set<string> expected;
    EXPECT_EQUAL(gatherTokens("!!! ... @#$"), expected);
}

STUDENT_TEST("gatherTokens handles repeated and surrounding spaces") {
    Set<string> expected = {"hello", "world"};
    EXPECT_EQUAL(gatherTokens("  hello   world  "), expected);
}

STUDENT_TEST("gatherTokens removes duplicates after cleaning") {
    Set<string> expected = {"hello", "123"};
    EXPECT_EQUAL(gatherTokens("Hello HELLO! hello 123 123!"), expected);
}

STUDENT_TEST("buildIndex stores all URLs for a shared word") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> expected = {
        "www.shoppinglist.com",
        "www.dr.seuss.net",
        "www.bigbadwolf.com"
    };
    EXPECT_EQUAL(index["fish"], expected);
}

STUDENT_TEST("buildIndex correctly cleans words and ignores empty tokens") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> expected = {"www.shoppinglist.com"};
    EXPECT_EQUAL(index["5lbmms"], expected);
    EXPECT(!index.containsKey("FISH"));
    EXPECT(!index.containsKey("milk,"));
    EXPECT(!index.containsKey(""));
}

STUDENT_TEST("buildIndex repeated words do not duplicate URLs") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> expected = {
        "www.rainbow.org",
        "www.dr.seuss.net"
    };
    EXPECT_EQUAL(index["red"], expected);
}

STUDENT_TEST("buildIndex timing") {
    Map<string, Set<string>> index;
    TIME_OPERATION(4, buildIndex("res/website.txt", index));
}

STUDENT_TEST("findQueryMatches cleans case and punctuation") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> expected = {"www.dr.seuss.net"};
    EXPECT_EQUAL(findQueryMatches(index, "RED +Fish!"), expected);
}

STUDENT_TEST("findQueryMatches processes terms left to right") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> expected = {"www.shoppinglist.com"};
    EXPECT_EQUAL(findQueryMatches(index, "red fish +milk"), expected);
}

STUDENT_TEST("findQueryMatches handles missing terms") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);

    Set<string> expected = {"www.rainbow.org", "www.dr.seuss.net"};
    EXPECT_EQUAL(findQueryMatches(index, "red -hippo"), expected);
    EXPECT(findQueryMatches(index, "red +hippo").isEmpty());
}