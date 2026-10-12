// TODO: remove and replace this file header comment
// This is a .cpp file you will edit and turn in.
// Remove starter comments and add your own
// comments on each function and on complex code sections.
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


// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
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

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
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

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
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

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query) {
    Set<string> result;
    // TODO: your code here
    return result;
}

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
void searchEngine(string dbfile) {
    // TODO: your code here
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


// TODO: add your test cases here

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