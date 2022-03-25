/**
 * @file anagram_dict.cpp
 * Implementation of the AnagramDict class.
 *
 * @author Matt Joras
 * @date Winter 2013
 */

#include "anagram_dict.h"

#include <algorithm> /* I wonder why this is included... */
#include <fstream>

using std::string;
using std::vector;
using std::ifstream;
using std::sort;
/**
 * Constructs an AnagramDict from a filename with newline-separated
 * words.
 * @param filename The name of the word list file.
 */
AnagramDict::AnagramDict(const string& filename)
{
    //vector<string> words;
    ifstream openFile(filename);
    string w;
    if(openFile.is_open()){
      while(getline(openFile,w)){
        string sword(w);
        sort(sword.begin(), sword.end());
        dict[sword].push_back(w);
      }
    }
    openFile.close();
      /* Your code goes here! */
}

/**
 * Constructs an AnagramDict from a vector of words.
 * @param words The vector of strings to be used as source words.
 */
AnagramDict::AnagramDict(const vector<string>& words)
{
    for(string w : words){
      string sword(w);
      sort(sword.begin(), sword.end());
      dict[sword].push_back(w);
    }


    /* Your code goes here! */
}

/**
 * @param word The word to find anagrams of.
 * @return A vector of strings of anagrams of the given word. Empty
 * vector returned if no anagrams are found or the word is not in the
 * word list.
 */
vector<string> AnagramDict::get_anagrams(const string& word) const
{
    vector<string> words;
    string sword(word);
    sort(sword.begin(), sword.end());
    if(dict.find(sword) == dict.end()){
      return vector<string>();
    }
    /* Your code goes here! */
    vector<string> v = dict.find(sword)->second;
    return v;
}

/**
 * @return A vector of vectors of strings. Each inner vector contains
 * the "anagram siblings", i.e. words that are anagrams of one another.
 * NOTE: It is impossible to have one of these vectors have less than
 * two elements, i.e. words with no anagrams are ommitted.
 */
vector<vector<string>> AnagramDict::get_all_anagrams() const
{
    vector<vector<string>> ret;
    //auto It = ret;
    for (auto it : dict){
      if(it.second.size() > 1){
        ret.push_back(it.second);
    }
  }
    return ret;
}
