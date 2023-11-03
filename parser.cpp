#include "parser.h"

// the function get info takes a block of information of one paper from source.txt
//and fills the fields of struct information of each paper
struct information getinfo(std::ifstream& f)
{
    struct information paper;
    paper.abstract="No abstract available";//in case the block has no #!
    paper.year=0;
    paper.venue="No venue data available";//in case the block has no #c
    std::string str;
    int ref_paper_id;
    while(true)
    {
        getline(f,str);//take one line from the source file
        if(str=="\0") break;// if it is empty line, it means we have reached the end of block
        if(str[1]=='i')
        {
            sscanf(str.c_str()+6,"%d",&paper.index);//index section
        }
        else if(str[1]=='*')
        {
            paper.title=str.erase(0,2);// tilte section
            if(paper.title.size()==0)
            paper.title="No title";
        }
        else if(str[1]=='@')
        {
            paper.authors=str.erase(0,2);
            if(paper.authors.size()==0)//authors section
            paper.authors="Anonymous Author";
        }
        else if(str[1]=='t')
        {
            sscanf(str.c_str()+2,"%d",&paper.year);//year section
        }
        else if(str[1]=='c')
        {
            paper.venue=str.erase(0,2);
            if(paper.venue.size()==0)// venue section
            paper.venue="No venue data available";
        }
        else if(str[1]=='!')
        {
            paper.abstract=str.erase(0,2);
            if(paper.abstract.size()==0)// abstract section
            paper.abstract="No abstract available";

        }
        else if(str[1]=='%')
        {
            if(str.size()>2){
            sscanf(str.c_str()+2,"%d",&ref_paper_id);
            // refernces section
            paper.ref_id.push_back(ref_paper_id);
            }
        }
    }
    return paper;
}


//split_name function takes a name as input and divides it into first, middle and last name
struct name split_name(std::string full_name,int priority)
{
    struct name author;
    int size=full_name.size();
    for(int i=0;i<full_name.size();i++)
    {
        //search until the first space and everything before it is first name and we edit the full name to
        //get middle and last name
        if(full_name[i]==' ')
        {
            author.first=full_name.substr(0,i);
            full_name=full_name.substr(i,full_name.size()-i);
            break;
        }
    }
    if(size==full_name.size())
    {
        //if there is no space, the entire name is the first name
        author.first=full_name;
        author.middle="\0";
        author.last="\0";
    }
    else
    {
        for(int i=full_name.size()-1;i>=0;i--)
        {
            // search from the end for a space and everything after it is last name
            // and the remaining part is middle name
            if(full_name[i]==' ')
            {
                author.last=full_name.substr(i+1,full_name.size()-i-1);
                author.middle=full_name.substr(1,i-1);
                if(i==0)
                author.middle="\0";
                break;
            }
        }
    }
        return author;
}


//split_authors take an entire string containing comma seperated author names as input and adds them to a vector of authors
//each element in the author is given to split_name as input to divide the name
//the authors are given preference in the order of their occurence
std::vector<struct name> split_authors(std::string all_names)
{
    std::vector<struct name> authors;
    int priority = 0;
    while(all_names.size()>0)
    {
        int count=0;
        for(int i=0;i<all_names.size();i++)
        {
            if(all_names[i]==',')
            {
                //if the string between commas is Jr., then it means it is part of the previous name and is added to the last name
                //of previous author
                if(all_names.substr(0,i)==" Jr.")
                authors[authors.size()-1].last+=", Jr.";
                else
                {
                    if(all_names.substr(0,i).size()>0)// when string between commas is empty, it is not added
                    {
                            authors.push_back(split_name(all_names.substr(0,i),priority));
                            priority++;
                    }
                }
                all_names=all_names.substr(i+1,all_names.size()-i-1);//edit the string and repeat the process
                break;
            }
            count++;
        }
        //if there is no comma, there is only one author left and it is added to authors vector
        if(count==all_names.size())
        {
            authors.push_back(split_name(all_names,priority));
            break;
        }
    }
    //we will check for repetitions before returning authors through clean function
    return authors;
}


//clean function takes the authors vector and removes authors who are repeated
std::vector<struct name> clean(std::vector<struct name> authors)
{
    
    for(int i=1;i<authors.size();i++)
    {
        for(int j=i-1;j>=0;j--)
        {
            if(authors[j].first==authors[i].first 
                && authors[j].middle==authors[i].middle && authors[j].last==authors[i].last)
            {
                for(int k=i+1;k<authors.size();k++)
                {
                    authors[k-1]=authors[k];
                }
                authors.pop_back();
                i--;
                break;
            }
        }
    }
    return authors;
}