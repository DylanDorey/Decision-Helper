/***************************************
Decision Helper
Author: Dylan Dorey
Date Completed: 11/15/24
Description: A program that processes an election given candidates and ballots parsed via XML files and determines a winner in a runoff fashion.
***************************************/

#include <iostream>
#include <string>
#include <array>
#include "HashTable.hpp"
#include "LinkedList.hpp"
#include "pugixml.hpp"
using namespace std;

//custom data type candidate for object instantiation
class Candidate
{
    private:

        //values for candidate's id, name, and if they were eliminated
        string id;
        string name;
        bool eliminated;

    public:

    //constructor for candidate object
    Candidate(string i, string n, bool e) : id(i), name(n), eliminated(e) {}

    //getter for candidate ID
    string getId()
    {
        return id;
    }

    //getter for candidate name
    string getName()
    {
        return name;
    }

    //getter for eliminated bool
    bool getEliminated()
    {
        return eliminated;
    }

    //setter for eliminated bool
    void setEliminated()
    {
        eliminated = true;
    }
};

//custom data type ballot object for object instantiation
class Ballot
{
    private:

        //Ids of three (3) ranked candidates, array of strings
        array<string, 3> candidateRanks;

    public:

    //constructor for ballot object
    Ballot(string h, string s, string t) : candidateRanks({h, s, t}) {}

    //returns id of highest ranked candidate id
    string getHighestRankedCandidate()
    {
        return candidateRanks.at(0);
    }

    //setter for the highest ranked candidate
    void setHighestRankedCandidate(int value)
    {
        candidateRanks.at(0) = candidateRanks.at(value);
    }
};

//Populates a Hash Table with all of the candidate's data parsed from the candidates XML file specified in command line argument
HashTable<Candidate> populateCandidatesHashTableFromFile(string &file)
{
    //hash table that will be populated and returned
    HashTable<Candidate> cHT;

    // Instantiate xml document variable and load passed in candidates xml file from command line argument
    pugi::xml_document cDoc;

    // Load the xml file from command line argument into the xml document variable
    // if a problem occurred while loading, show error
    if (!cDoc.load_file(file.c_str()))
    {
        cout<<"Problem opening xml file "<<file<<endl;
    }

    // Get the root node in the XML file
    pugi::xml_node root = cDoc.first_child();

    // Loop through all "candidate" elements in the XML file
    for (pugi::xml_node xmlCandidate : root.children("candidate"))
    {
        // Retrieve the "id" attribute from the current <candidate> node
        // Retrieve the "name" element from current <candidate> node
        string id = xmlCandidate.attribute("id").as_string();
        string name = xmlCandidate.child("name").child_value();

        // Instantiate Candidate object using values retrieved from XML file
        Candidate candidate(id, name, false);

        // add the candidate as a value to the hash table of candidates using the candidate's id as its key
        cHT.add(id, candidate);
    }

    //return the populated hash table of candidates
    return cHT;
}

//Populates a Linked List with all of a ballot's data parsed from the ballots XML file specified in command line argument
LinkedList<Ballot> populateBallotsListFromFile(string &file)
{
    //linked list that will be populated and returned
    LinkedList<Ballot> bList;

    // Instantiate xml document variable and load passed in candidates xml file
    pugi::xml_document bDoc;

    // Load the xml file from command line argument into the xml document variable
    // if a problem occurred while loading, show error
    if (!bDoc.load_file(file.c_str()))
    {
        cout<<"Problem opening xml file "<<file<<endl;
    }

    // Get the root node in the XML file
    pugi::xml_node xmlBallots = bDoc.first_child();

    // Loop through all "ballot" elements in the XML file
    for (pugi::xml_node xmlBallot : xmlBallots.children("ballot"))
    {
        //strings to store the ids of each ranked candidate
        string id1, id2, id3;

        // Loop through all "vote" children nodes in the ballot node
        for (pugi::xml_node vote : xmlBallot.children("vote"))
        {
            //the rank position for each candidate
            int candidateRank = vote.attribute("rank").as_int();

            //switch for the chosen rank
            switch (candidateRank)
            {
                //if 1, set as id1
                case 1:
                    id1 = vote.child_value();
                    break;
                //if 2, set as id2
                case 2:
                    id2 = vote.child_value();
                    break;
                //if 3, set as id3
                case 3:
                    id3 = vote.child_value();
                    break;
            }
        }

        //create a new ballot object using the initialized id strings and populat the ballot's candidate rank array
        Ballot newBallot(id1, id2, id3);

        //push that new ballot to the back of the linked list
        bList.pushBack(newBallot);
    }

    //return the populated linked list of ballots
    return bList;
}

//populates a hash table with tallied votes of all candidates using the algorithm discussed during lecture
HashTable<int> tallyRoundVotes(LinkedList<Ballot> &ballots, HashTable<Candidate> &candidates)
{
    //Instantiate hash table to hold tallied votes
    HashTable<int> talliedVotesHT;

    // Create alias for hash table iterator to make life easier
    using hashIter = typename HashTable<Candidate>::Iterator;

    //iterate through the candidates hash table
    for(hashIter iter = candidates.generateIterator(); iter.hasNext(); iter.next())
    {
        //if the candidate isnt eliminated
        if(!iter.current()->second.getEliminated())
        {
            //add the candidate to the talliedVotesHT hash table with a starting vote count of 0
            talliedVotesHT.add(iter.current()->first, 0);
        }
    }


    // Create alias for linked list iterator to make life easier
    using listIter = typename LinkedList<Ballot>::Iterator;

    //iterate through the ballots linked list
    for(listIter iter = ballots.generateIterator(); iter.hasNext(); iter.next())
    {
        //create an index for the ballot's vote/rank position
        int index = 1;

        //if the current candidate that is the highest ranked is eliminated
        while (candidates.get(iter.current()->getHighestRankedCandidate())->getEliminated())
        {
            //set the highest ranked candidate to the next highest rated candidate in the running and increase the index
            iter.current()->setHighestRankedCandidate(index);
            index++;
        }

        //increment the number of votes that the current ballot's highest ranked candidate has from the talliedVotesHT hash table
        int *voteCount = talliedVotesHT.get(iter.current()->getHighestRankedCandidate());
        (*voteCount)++;
    }

    //return the hash table containing tallied votes 
    return talliedVotesHT;
}

//Processes each round's results based upon the tallied votes hash table to eventually determine a winner in a runoff style
void processRoundResults(HashTable<int> &talliedVotes, HashTable<Candidate> &candidates, float &halfOfVotes, string &win)
{   
    //generate an iterator for the tallied votes hash table
    HashTable<int>::Iterator iter = talliedVotes.generateIterator();

    // assume first candidates id in list is the loser
    string loser = iter.current()->first;

    //while the hash table has another vot count and the current selected candidate isn't already eliminated
    while (iter.hasNext() && !candidates.get(iter.current()->first)->getEliminated())
    {
        //display the candidates name and votes recieved by getting the id key of the candidate from the talliedVotes hash table
        cout<<candidates.get(iter.current()->first)->getName()<<": "<<iter.current()->second<<" vote(s)"<<endl;

        //find winner by determining if the current candidates votes are greater than half of the votes
        if(*talliedVotes.get(iter.current()->first) > halfOfVotes)
        {
            //set winner to the current candidates name
            win = candidates.get(iter.current()->first)->getName();
        }
        //otherwise if the current candidates votes are less than the current losers votes
        else if (iter.current()->second < *talliedVotes.get(loser))
        {
            //set the loser to the current candidate
            loser = iter.current()->first;
        }

        //move to the next candidates votes
        iter.next();
    }
    
    //eliminate the loser of the round
    candidates.get(loser)->setEliminated();
}

int main(int argc, char* argv[]) 
{
    // Simple validation for command line arguments
    if (argc != 3)
    {
        cerr<<"Must supply two command line argument to program!\n";
        return 1;
    }

    //parse candidates XML file specified in command line argument
    string candidatesFile = argv[1];

    //parse ballot XML file specified in command line argument
    string ballotsFile = argv[2];
    
    //instantiate object of type "HashTable" that is populated with all of the candidate data parsed from candidates XML file specified in command line argument
    HashTable<Candidate> candidatesHT = populateCandidatesHashTableFromFile(candidatesFile);

    //instantiate object of type "LinkedList" that is populated with all of the ballot data parsed from ballots XML file specified in command line argument
    LinkedList<Ballot> ballotsList = populateBallotsListFromFile(ballotsFile);

    //Determine the number of total ballots by getting the size of the ballots linked list
    int totalBallots = ballotsList.size();
    
    //Determine what half of the votes would be and display it to the CLI
    float halfOfVotes = float(totalBallots)/2;
    cout<<"\nTotal Ballots: "<<totalBallots<<endl;

    //values for the round number and the winner
    int roundNum = 0;
    string winner;

    //while there is not a winner/the winner string is empty
    while(winner.empty())
    {
        //increase the round number and display what round it is
        roundNum++;
        cout<<"\nRound "<<roundNum<<" Runoff Results:"<<endl;

        //instantiate object(s) of type "HashTable" that are populated with tallied votes of all candidates using algorithm discussed during lecture
        HashTable<int> talliedVotes = tallyRoundVotes(ballotsList, candidatesHT);

        //Process each round's results with the caluclated values
        processRoundResults(talliedVotes, candidatesHT, halfOfVotes, winner);
    }

    //display winner
    cout<<endl<<"The winner is "<<winner<<endl;

    return 0;
}