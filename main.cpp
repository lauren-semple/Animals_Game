//Lauren Semple 11/17/19
/* This program plays an animal guessing game. When the animal is guessed
correctly the programs wins, when the program is unable to guess the animal
it asks for a distinguishing question about the animal and that animal is
added to the program's knowledge base. */

#include <iostream>
#include <fstream>
#include <string>
#include <deque>

using namespace std;


struct Tree_node
{
  string question;
  struct Tree_node *left = nullptr;
  struct Tree_node *right = nullptr;
};

//Declaration Section
//This function takes an integer and converts it into a binary address for a tree node
deque<unsigned> finds_address (unsigned n);
//This function interprets a node's binary address and inserts it into the binary tree
void constructs_binary_tree (Tree_node *& tree, deque<unsigned> address,
                             string question, ifstream& input_file);
//This function reads a pre-order tree traversal from a file
void reads_from_file (Tree_node *& tree, ifstream &input_file);
//This function writes a pre-order tree traversal to a file
void writes_to_file( Tree_node *tree, unsigned n, ofstream &output_file);
//This function generates and places a new node for a new animal into the binary tree
void insert_new_animal (Tree_node *& current, Tree_node *& previous);
//This function runs through the questions in the game, excluding the final question
void asks_question (Tree_node *& current, Tree_node *& previous);
//This function asks the final "Is it this animal" question and handles the response
void guesses_animal(Tree_node *& current, Tree_node *& previous);
//This functions returns true if the player would like to play again
bool play_again();
//This function deletes the dynamically allocated tree
void deallocates_tree(Tree_node *& tree);

int main()
{
  cout << "Are you ready to play the game?" << endl;
  cout << "Think of an animal and I'll try to guess which one!" << endl;
  cout << endl;

  Tree_node *previous = nullptr;
  Tree_node *current = nullptr;

  ifstream input_file;
  input_file.open ("animals.txt");
  if (input_file.fail() )
  {
    cout << "Attempt to open file failed.";
  }
  reads_from_file(current, input_file);
  input_file.close();

  bool repeat = true;

  while (repeat == true) //repeats while player still wants to play
  {
  asks_question(current, previous);

  ofstream output_file;
  output_file.open("animals.txt");
  writes_to_file(current, 1, output_file);
  output_file.close();

  repeat = play_again();
  }

  deallocates_tree(current);

  return 0;
}

deque<unsigned> finds_address (unsigned n)
{
  deque<unsigned> address;
  while (n > 1)
  {
    unsigned r;
    r = n % 2;
    address.push_front (r);
    n = n / 2;
  }
  return address;
}

void constructs_binary_tree (Tree_node *& tree, deque<unsigned> address,
                             string question, ifstream& input_file)
{
  if (address.empty()) //base case when whole address has been read
  {
    Tree_node *new_tree_node = new Tree_node;
    new_tree_node->question = question;
    tree = new_tree_node;
    return;
  }
  else if (address.front() == 0)
  {
    address.pop_front();
    constructs_binary_tree (tree->left, address, question, input_file);
  }
  else if (address.front() == 1)
  {
    address.pop_front();
    constructs_binary_tree (tree->right, address, question, input_file);
  }
}

void reads_from_file (Tree_node *& tree, ifstream &input_file)
{
  unsigned n;
  string question;

  while(input_file.good())
  {
    input_file >> n;
    string dummy;
    getline (input_file, dummy);
    getline(input_file, question);
    deque<unsigned> address = finds_address (n);
    constructs_binary_tree (tree, address, question, input_file);
  }
}

void writes_to_file(Tree_node *tree, unsigned n, ofstream &output_file)
{
  if (tree != nullptr) //base case when node has no children
  {
    output_file << n << endl;
    output_file << tree->question << endl;
    writes_to_file (tree->left, (2*(n)), output_file);
    writes_to_file (tree->right, (2*(n)+1), output_file);
  }
}

void insert_new_animal (Tree_node *& current, Tree_node *& previous)
{
  cout << "What's your animal? ";
  string answer = "Is it ";
  string input;
  cin.ignore();
  getline(cin, input);

  cout << endl;

  cout << "I need a yes or no question that will distinguish "
       << input << " from my guess: " << endl;
  cout << "What is that question? ";
  string question;
  getline(cin, question);
  cout << endl;
  cout << "What is the correct answer for " << input << "?" << endl;
  cout << "Please type y for yes or n for no. ";
  string y_or_n;
  getline(cin, y_or_n);

  answer.append(input);
  answer.push_back('?');

  if (y_or_n == "y")
  {
   Tree_node *yes_answer = new Tree_node;
   yes_answer->question = answer;
   yes_answer->left = nullptr;
   yes_answer->right = nullptr;

   Tree_node *new_question = new Tree_node;
   new_question->question= question;
   new_question->right = yes_answer;
   new_question->left = current;

   if(previous->left == current)
    {
        previous->left = new_question;
    }

    else if(previous->right == current)
    {
        previous->right = new_question;
    }
    else
    {
      current = new_question;
    }
  }
  else if (y_or_n == "n")
  {
   Tree_node *no_answer = new Tree_node;
   no_answer->question = answer;
   no_answer->left = nullptr;
   no_answer->right = nullptr;

   Tree_node *new_question = new Tree_node;
   new_question->question= question;
   new_question->right = current;
   new_question->left = no_answer;

   if(previous->left == current)
    {
        previous->left = new_question;
    }

    else if(previous->right == current)
    {
        previous->right = new_question;
    }
    else
    {
      current = new_question;
    }
  }

    cout << "Thanks! I've updated my knowledge." << endl << endl;

}

void guesses_animal(Tree_node *& current, Tree_node *& previous)
{
  char choice;
  cout << current->question << "(y/n) ";
  cin >> choice;

  if (choice == 'y')
  {
    cout << "Yay! I guessed correctly. " << endl;
  }
  else if (choice == 'n')
  {
    cout << "You stumped me! Please help me learn more animals." << endl;
    insert_new_animal(current, previous);
  }
  else
  {
    cout<<"Please enter a valid answer y or n"<<endl;
    guesses_animal(current, previous);
  }
}

void asks_question (Tree_node *& current, Tree_node *& previous)
{
  char choice;

  if (previous == nullptr)
  {
    previous = current;
  }

  if(current->left!=nullptr && current->right!=nullptr)
  {
    cout << current->question << "(y/n) ";
    cin >> choice;

    if(choice == 'y')
    {
      asks_question(current->right, current);
    }
    else if (choice == 'n')
    {
      asks_question(current->left, current);
    }
    else
    {
      cout<<"Please enter a valid answer y or n"<<endl;
      asks_question(current,previous);
    }
  }
  else //base case when reaches a leaf node, e.i final question
  {
    guesses_animal(current, previous);
  }
}

bool play_again()
{
  cout << "Would you like to play again?(y/n) ";
  char y_or_n;
  cin >> y_or_n;

  cout << endl;

  if (y_or_n == 'y')
  {
    cout << "Great! I will reboot the game. " << endl;
    cout << endl;
    return true;
  }
  else if (y_or_n == 'n')
  {
    cout << "Okay, thanks for playing!" << endl;
    return false;
  }
  else
  {
    cout << "Invalid response. Ending game." << endl;
    return false;
  }
}

void deallocates_tree(Tree_node *& tree)
{
    if (tree == nullptr) //base case when node has no children
    {
      return;
    }
    else
    {
      deallocates_tree(tree->left);
      deallocates_tree(tree->right);
      delete tree;
      tree = nullptr;
    }
}


