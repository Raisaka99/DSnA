#include <iostream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>
#include <algorithm>
#include <queue>
#include <list>
//#include <bits/stdc++.h>
using namespace std;
#define string std::string
#define cout std::cout
#define endl std::endl;
using namespace std::tr1;





bool isDup(vector<int>& v)
{
                //key, value(count)
    unordered_map<int, int> m;

    for(int i = 0; i < v.size(); i++)
    {
        if(m.find(v[i]) != m.end())
            return true;
        m[v[i]] = 1;
    }
    return false;
}

/*
    another possible solution would be to sort the strings and the compare them if
    if they are equal to each other. 
*/
bool isAnagram(string a, string b)
{
    if(a.size() != b.size())
        return false;

    unordered_map<char, int> j;
    unordered_map<char, int> k;

    
    for(int i = 0; i < a.size(); i++)
    {
        j[a[i]]++; //increase the value at the specific key 
        k[b[i]]++; //same thing
    }

    for(int i = 0; i < j.size(); i++)
    {
        if(j[i] != k[i])
            return false;
    }
    return true;
}

vector<int> twosum(vector<int>& v, int target)
{
    vector<int> found;
    unordered_map<int, int> m;

    for(int i = 0; i < v.size(); i++)
    {
        int diff = target - v[i];//diff will be what we are looking for in the hash map to find a match
        if(m.find(diff) != m.end())//we found the difference in our hash map 
        {
            found.push_back(i);//push the index of the first value into the vector
            found.push_back(m[diff]);//push the index of the key from the hash map into the vector
            return found;//return the vector with the indeces 
        }
        m[v[i]] = i;//if not found in the hashmap yet, add the key and the index to the map
    }
    return found;//nothing is found, we return an empty vector
}

string getKey(string s)
{
    vector<int> count(26);//this vector will look like this count[0, 0, 0, 0, ..., 0] with 26 slots of the alphabet
    for(int i = 0; i < s.size(); i++)//we can go through the string
    {
        count[s[i] - 'a']++;//if the character(s[i]) = a then it will look like this --> count['a'-'a']++ which is count[0]++
        //so at index 0 becuase of the char 'a', we can increment the first inddex of the vector to 1
        //now the vector looks likes this count[1, 0, 0, 0, ..., 0] with the first index being a with 1 occurence
    }
    string key = "";//stores the key with a pattern of the count vector
    for(int i = 0; i < 26; i++)//go through the for loop 26 because thats the size of the vector
    {
        cout << "this is count " << count[i] << endl;
        key.append(to_string(count[i] + 'a'));//add to the key
    }
    return key;
}
vector<vector<string>> groupAnagram(vector<string>& V)
{
    unordered_map<string, vector<string>> m;//make a hashmap with the key being the pattern in getKey() and the value a being the string in the original vector
    for(int i = 0; i < V.size(); i++)
    {
        string key = getKey(V[i]);//will make a key for each string in the vector["bat", "cat", "rat"...]
        cout << "this is the key " << key << endl;
        m[key].push_back(V[i]);//the key(pattern) will go inside the hashmap and the original string("bat") will be the value 
    }

    vector<vector<string>> res;//will be returned as a matrix basically or a vetor of vectors inside
    for(const auto& iterator : m)//= m.begin(); iterator != m.end(); iterator++)//start with an auto pointer to the begin of the map that goes to the end
    {
        res.push_back(iterator.second);// iterator->second//we can access the second value of a hashmap using the -> or . operator map<string, vector<string>>
        //we can then push that vector into our vector that holds other vectors 
    }
    return res;
}
static bool compare(pair<int, int>& a, pair<int, int>& b)
{
    return a.second > b.second;
}
vector<int> topKFrequent(vector<int>& nums, int k)
{
    unordered_map<int, int> m;
    vector<pair<int, int>> t;
    vector<int> res;

    for(int i = 0; i < nums.size(); i++)
    {
        m[nums[i]]++;
    }

    for(auto& it: m)
    {
        t.push_back(it);
    }

    sort(t.begin(), t.end(), compare);


    for(auto& it : t)
    {
        
        if(k > 0)
        {
            res.push_back(it.first);   
        }
        k--;
        if(k == 0) break;
    }
    //return res;
    
    for(int i = 0; i < res.size(); i++)
    {
        cout << res[i] << " ";
    }
    
   return res;
  /*  
    for(auto& it : t)
    {
        cout << it.first << " " << it.second << endl;
    }
*/
}
vector<int> productExceptSelf(vector<int>& nums)
{
    return nums;
    //pain
}
int longestConsecutive(vector<int>& nums)
{
    unordered_set<int>s(nums.begin(), nums.end());
        int longest = 0;
        for(auto &n: s){
            //if this is the start of the sequence
            if(!s.count(n - 1)){
                cout << "this is n: " << n << endl;
                int length = 1; 
                while(s.count(n + length))
                    ++length;
                longest = max(longest, length);
            } 

        }
        return longest;
}
//Two pointers
int maxArea(vector<int> height)
{
    int left = 0;
    int right = height.size() - 1;
    //int width = (right+1) - left;
    int MAXAREA = 0;
    int LorR = 0;
    //height will be determined by which pointer is smaller left or right

    while(left < right)
    {
        //int width = (right) - left;
        //cout << "this is the width " << width << " ";
        MAXAREA = (right - left) * min(height[left], height[right]);
        LorR = max(MAXAREA, LorR);
        //cout << MAXAREA << endl;
        if(height[left] > height[right])
        {
            right--;
        } 
        else
        {
            left++;
        }
    }
    return LorR;
}
//sliding window
bool isInString(string s, char j)
{
    for(int i = 0; i < s.size(); i++)
    {
        if(j == s[i])
            return true;
    }
    return false;
}
int lengthOfLongestSubstring(string s) 
{
    int count = 0;
    unordered_set<char> sv;
    int left = 0;

    for(int right = 0; right < s.size(); right++)
    {
        while(sv.find(s[right]) != sv.end())
        {
            sv.erase(s[left]);
            ++left;
        }
        
    }

    
}
int characterReplacement(string s, int k) 
{
    vector<int> count(26);
    int res = 0;
    int left = 0; 
    int right = 0;
    int maxC = 0;

    while(right < s.size())
    {
        count[s[right] - 'A']++;//increment index of given character
        maxC = max(maxC, count[s[right] - 'A']);

        if(right - left + 1 - maxC > k)//if our window is no longer valid we havve to shift it to the right using oyur left pointer
        {
            count[s[left] - 'A']--;//we have to decrement the count of the charcater at that index since it is no longer in our window
            left++;//shift left pointer to the right
        }
        res = max(res, right-left+1);//this is the length of our window so far as we go through the string 
        right++;//incremenent right pointer
    }
    return res;
}
bool checkInclusion(string s1, string s2) 
{
    if(s1.size() > s2.size())
        return false;
            
}
int main()
{
    vector<int> V = {0,3,7,2,5,8,4,6,0,1};
    vector<int> VV = {1, 2, 3, 4, 2};
    vector<string> yy = {"eat","tea","tan","ate","nat","bat"};
    vector<int> sus = {1, 1, 1, 2, 2, 3, 3, 3};
    vector<int> huh = {1, 1};
    vector<int> area = {1,8,6,2,5,4,8,3,7};

    cout << lengthOfLongestSubstring("dvdf") << endl;
    //cout << maxArea(huh) << endl;
    //vector<vector<string>> ayo = groupAnagram(yy);
    //int total = longestConsecutive(huh);
    //cout << total << endl;
    //topKFrequent(huh, 1);

 /*
    for(int i = 0; i < ayo.size(); i++)
    {
        for(int j = 0; j < ayo[i].size(); j++)
            cout << ayo[i][j] << " ";
    }
    
    vector<int> re = twosum(V, 7);
    for(int i = 0; i < re.size(); i++)
        cout << re[i] << endl;
    if()
        cout << "Is anagram";
    else
        cout << "NOT anagram";
    */

    
    return 0;
}
