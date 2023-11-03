#include "parser.h"
#include <map>
#include <iterator>
#include<pqxx/pqxx>

// opening the data base named postgres of user postgres. password for user postgres is "cs20btech11049"
// host address is 127.0.0.1 port= 5432
// If you want to run this code please change the password to your password.
pqxx::connection C("user = postgres password = cs20btech11049 \
      hostaddr = 127.0.0.1 port = 5432");


// Queries to create tables with desired attributes and primary keys
void create_tables()
{
    std::string query;
    pqxx::work create_table(C);
    query = "CREATE TABLE Paper("  \
      "ID             INT PRIMARY KEY     NOT NULL," \
      "NAME           VARCHAR    NOT NULL," \
      "YEAR           INT  , " \
      "ABSTRACT       VARCHAR);";

    create_table.exec(query);
    printf("Created Paper table successfully\n");

    // serial implies that new key is generated for primary key value
    query = "CREATE TABLE Author("  \
      "A_ID           SERIAL PRIMARY KEY ," \
      "FIRST_NAME     VARCHAR    NOT NULL," \
      "MIDDLE_NAME    VARCHAR  , " \
      "LAST_NAME      VARCHAR);";

    create_table.exec(query);
    printf("Created AUTHOR table successfully\n");

    query = "CREATE TABLE Venue("  \
      "V_ID           SERIAL PRIMARY KEY  ," \
      "NAME           VARCHAR);";

    create_table.exec(query);
    printf("Created Venue table successfully\n");

    query = "CREATE TABLE Reference("  \
      "R_ID INT       NOT NULL," \
      "S_ID INT       NOT NULL," \
      "PRIMARY KEY(R_ID, S_ID)  );";

    create_table.exec(query);
    printf("Created Reference table successfully\n");

    query = "CREATE TABLE Pap_Authors("  \
      "Pap_ID INT     NOT NULL," \
      "Aut_ID INT     NOT NULL," \
      "PREFERENCE     INT    NOT NULL," \
      "PRIMARY KEY(Pap_ID, Aut_ID)  );";

    create_table.exec(query);
    printf("Created author_info table successfully\n");

    query = "CREATE TABLE Pap_Ven_info("  \
      "Pap_ID INT     NOT NULL," \
      "Ven_ID INT     NOT NULL," \
      "PRIMARY KEY(Pap_ID, Ven_ID)  );";

    create_table.exec(query);
    printf("Created Ven_info table successfully\n");

    // commiting all the newly created tables to database.
    create_table.commit();
}


// Preparing the prepare statements to enter values into 6 tables created above
void prepare_queries()
{
    // Prepare statement to enter values into Paper table.
    C.prepare(
        "paper_enter",
        "INSERT INTO Paper "\
        "VALUES ($1, $2, $3, $4);");

    // Prepare statement to enter values into Author table.
    C.prepare(
        "author_enter",
        "INSERT INTO Author (A_ID,FIRST_NAME,MIDDLE_NAME,LAST_NAME)"\
        "VALUES (DEFAULT,$1, $2, $3)"\
        "RETURNING A_ID;");
        
    // Prepare statement to enter values into Pap_Authors table.
    C.prepare(
        "pap_author",
        "INSERT INTO Pap_Authors "\
        "VALUES ($1, $2, $3);");

    // Prepare statement to enter values into Venue table.
    C.prepare(
        "venue_enter",
        "INSERT INTO Venue (V_ID, NAME)"\
        "VALUES (DEFAULT,$1)"\
        "RETURNING V_ID;");

    // Prepare statement to enter values into Pap_Ven_info table.
    C.prepare(
        "pap_ven_enter",
        "INSERT INTO Pap_Ven_info "\
        "VALUES ($1, $2);");

    // Prepare statement to enter values into Reference table.
    C.prepare(
        "ref_enter",
        "INSERT INTO Reference "\
        "VALUES ($1, $2);");

}



int main()
{
    // Checking if successfully connected to the data base
    if (C.is_open()) 
    {
        std::cout << "Opened database successfully: " << C.dbname() << std::endl;
    } 
    else 
    {
        std::cout << "Can't open database" << std::endl;
        return 1;
    }
    // setting the encoding to UTF-8 so that DB can support multilingual text.
    C.set_client_encoding("utf8");

    // creating map data structures to hold the info about the papers authora and venues entered till now.
    std::map<struct name, int> author;
    std::map<struct pap_name, int> paper;
    std::map<struct venue_name, int> venue;

    // Holds no of papers to enter in to DB
    int no_of_papers;

    // Input file object
    std::ifstream fp;

    fp.open("source.txt");
    if(fp.is_open())
    printf("source.txt file opened successfully \n");
    else
    {
        printf("Failed to open source.txt file\n");
        return (1);
    }

    fp>>no_of_papers;
    std::string str;
    // reading \n charecter for reading to procees smoothly.
    getline(fp,str);
    printf("%d\n",no_of_papers);

    //creating tables
    create_tables();
    // preparing tables
    prepare_queries();

    // Entering basic rows like Anonymous author and No Venue data available in to Author and Venue.
    pqxx::work W1(C);
    W1.prepared("author_enter")("Anonymous")("")("Author").exec();
    W1.prepared("venue_enter")("No venue data available").exec();
    W1.commit();

    // Entering the these basic info into maps.
    std::pair<struct name,int> p((name{"Anonymous","","Author"}),1);
    author.insert(p);
    std::pair<struct venue_name,int> p1((venue_name{"No venue data available"}),1);
    venue.insert(p1);

    // enetring each paper info in to respective tables
    int i,j;
    pqxx::work insert(C);
    // used to get the id of newly entered row
    pqxx::result r;

    for(i=0;i<no_of_papers;i++)
    {
        // calling the parser and take the input of a single paper.
        struct information curr_paper=getinfo(fp);
        printf("Entered paper %d\n",curr_paper.index);

        // creating a pap_name struct object sothat the info can be put in the map
        struct pap_name curr_paper_name;
        curr_paper_name.p_name=curr_paper.title;
        curr_paper_name.aut_str=curr_paper.authors;
        std::pair<struct pap_name, int> pap_p(curr_paper_name,curr_paper.index);

        // Checking if the paper with same name is already entered in to DB
        auto pap_it=paper.find(curr_paper_name);

        // If there is no paper with same name then it is entered in to DB
        if(pap_it == paper.end())
        {
            // entering paper into map
            paper.insert(pap_p);
            //also entering it in to DB;
            insert.prepared("paper_enter")(curr_paper.index)(curr_paper.title)(curr_paper.year)(curr_paper.abstract).exec();

        }
        // else if paper with same name exists then
        else
        {
            // if both papers have same name then skiping entering of this paper in to DB.
            if(pap_it->first.aut_str==curr_paper.authors) continue;
            // else if the authors are different then entering it ino both DB and map
            else
            {
                insert.prepared("paper_enter")(curr_paper.index)(curr_paper.title)(curr_paper.year)(curr_paper.abstract).exec();
                paper.insert(pap_p);

            }

        }
        // spliting the authors names and removing repetition of same author name more than once or null authors
        std::vector<name> author_info= split_authors(curr_paper.authors);
        author_info=clean(author_info);

        // holds the id's of authors who wrot e the current paper to crate the pap_authors relation table
        std::vector<int> aut_ids;

        for(j=0;j<author_info.size();j++)
        {
            // Checkin if the author is already in DB
            auto aut_it=author.find(author_info[j]);
            int a_id;
            // If not present in DB then enetr in to both the DB and map
            // and also note the a_id of the author
            if(aut_it==author.end())
            {
                // insert into DB;
                r=insert.prepared("author_enter")(author_info[j].first)(author_info[j].middle)(author_info[j].last).exec();
                a_id=r.begin().begin().as<int>();
                // taking the id
                aut_ids.push_back(a_id);
                // inserting into map
                std::pair<struct name, int> aut_p(author_info[j],a_id);
                author.insert(aut_p);
            }
            // If the author is already in DB then take his id and push it into vector aut_ids.
            else 
            {
                a_id=aut_it->second;
                aut_ids.push_back(a_id);
            }
        }

        // Building Pap_Authors relation table based on the above collected ids.
        for(j=0;j<aut_ids.size();j++)
        {
            insert.prepared("pap_author")(curr_paper.index)(aut_ids[j])(j).exec();
        }

        struct venue_name curr_ven_name;
        curr_ven_name.ven_name=curr_paper.venue;
        int ven_id;
        // Checking if the Venue or venue is already in the DB 
        auto ven_it = venue.find(curr_ven_name);
        // If no entering it to the database and into the map while collecting its primary key value.
        if(ven_it==venue.end())
        {
            //enter in to DB and get Ven_id
            r=insert.prepared("venue_enter")(curr_paper.venue).exec();
            ven_id=r.begin().begin().as<int>();
            // entering into the map
            std::pair<struct venue_name, int> ven_p(curr_ven_name,ven_id);
            venue.insert(ven_p);
        }
        // If the venue is already present then simply get its id
        else
        {
            ven_id=ven_it->second;
        }
        // Filling the Pap_Ven_info table using the id colected above
        insert.prepared("pap_ven_enter")(curr_paper.index)(ven_id).exec();

        // Filling the Reference table using the references taken via parsing
        for(j=0;j<curr_paper.ref_id.size();j++)
        {
            if(curr_paper.index!=curr_paper.ref_id[j])
            insert.prepared("ref_enter")(curr_paper.index)(curr_paper.ref_id[j]).exec();
        }

    }
    // Commiting all the papers data enterd above
    insert.commit();
    // if controll reaches here that means the program finished successfully
    printf("Building Database Finished Successfully\n");
    return(0);
}