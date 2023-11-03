// libraries required for both parser and loader
#include <iostream>
#include <string>
#include<fstream>
#include<vector>

// struct to hold all the data related to a research paper
struct information
{
    std::string title;
    std::string authors;
    int year;
    int index;
    std::string venue;
    std::string abstract;
    std::vector<int> ref_id;
};

// holds the FULL NAME of a author
struct name 
{
    std::string first;
    std::string middle;
    std::string last;

// operator over load to use this as a key in map
    bool operator==(const name &n) const {
        return first==n.first && middle==n.middle && last==n.last;
    }
    bool operator < (const name &n) const {
        if(first<n.first) return true;
        else if(first>n.first) return false;
        else if(first == n.first)
        {
            if(middle<n.middle) return true;
            else if(middle>n.middle) return false;
            else if(middle== n.middle)
            {
                if(last<n.last) return true;
                else if(last>n.last) return false;
                else return false;
            }
        }
        return false;
    }
};

// used to maintain a map of papers entered till now and their authors
struct pap_name
{
    std::string p_name;
    std::string aut_str;
    bool operator==(const pap_name &n) const {
        return p_name==n.p_name;
    }
    bool operator < (const  pap_name&n) const {
        return p_name<n.p_name;
    }

};

// used as a key to map holding all the venues entered till now
struct venue_name
{
    std::string ven_name;
    bool operator==(const venue_name &n) const {
        return ven_name==n.ven_name;
    }
    bool operator < (const  venue_name&n) const {
        return ven_name<n.ven_name;
    }

};

// functions used for parsing and sorting out authors names
struct information getinfo(std::ifstream& f);
struct name split_name(std::string full_name,int priority);
std::vector<struct name> split_authors(std::string all_names);
std::vector<struct name> clean(std::vector<struct name> authors);
