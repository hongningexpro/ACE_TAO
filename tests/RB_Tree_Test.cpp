// $Id$

// ============================================================================
//
// = LIBRARY
//    tests
//
// = FILENAME
//    RB_Tree_Test.cpp
//
// = DESCRIPTION
//    This is a test to verify and illustrate the use of the ACE_RB_Tree
//    and ACE_RB_Tree_Iterator classes.  Two different key and item types are
//    used in order to demonstrate specialization of the ACE_Less_Than
//    comparison function object template: int (for which the native <
//    operator is sufficient), and const char * (for which < operator
//    semantics must be replaced by strcmp semantics).  An RB tree for each of
//    the four possible type parameter permutations over int and const char *
//    is constructed and filled in, and the resulting order is checked via an
//    iterator over each.
//
// = AUTHOR
//    Chris Gill <cdgill@cs.wustl.edu>
//
// ============================================================================

#include "test_config.h" /* Include first to enable ACE_ASSERT. */
#include "ace/RB_Tree.h"

ACE_RCSID(tests, RB_Tree_Test, "$Id$")

#if defined(__BORLANDC__) && __BORLANDC__ >= 0x0530
USELIB("..\ace\aced.lib");
//---------------------------------------------------------------------------
#endif /* defined(__BORLANDC__) && __BORLANDC__ >= 0x0530 */

// Type definitions for the four distinct parameterizations of ACE_RB_Tree
// and its iterators.

typedef ACE_RB_Tree<int, int, ACE_Less_Than<int>, ACE_Null_Mutex> INT_INT_RB_TREE;
typedef ACE_RB_Tree<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex> INT_STR_RB_TREE;
typedef ACE_RB_Tree<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex> STR_INT_RB_TREE;
typedef ACE_RB_Tree<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex> STR_STR_RB_TREE;

typedef ACE_RB_Tree_Iterator<int, int, ACE_Less_Than<int>, ACE_Null_Mutex> INT_INT_FWD_ITER;
typedef ACE_RB_Tree_Iterator<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex> INT_STR_FWD_ITER;
typedef ACE_RB_Tree_Iterator<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex> STR_INT_FWD_ITER;
typedef ACE_RB_Tree_Iterator<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex> STR_STR_FWD_ITER;

typedef ACE_RB_Tree_Reverse_Iterator<int, int, ACE_Less_Than<int>, ACE_Null_Mutex> INT_INT_REV_ITER;
typedef ACE_RB_Tree_Reverse_Iterator<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex> INT_STR_REV_ITER;
typedef ACE_RB_Tree_Reverse_Iterator<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex> STR_INT_REV_ITER;
typedef ACE_RB_Tree_Reverse_Iterator<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex> STR_STR_REV_ITER;


// These arrays of numbers as ints and character strings
// are used to instantiate key and item nodes in the tree.

static const char *number_strings [] =
{
  "10", "20", "30", "40", "50", "60", "70", "80"
};

static int number_integers [] =
{
  10, 20, 30, 40, 50, 60, 70, 80
};

// These arrays of ints are used to shuffle the order of insertion
// of keys and items for the various trees.
static int int_int_index [] = {0, 1, 2, 3, 4, 5, 6, 7}; // LR inorder
static int int_str_index [] = {7, 6, 5, 4, 3, 2, 1, 0}; // RL inorder
static int str_int_index [] = {4, 6, 2, 7, 5, 3, 1, 0}; // RL BFS
static int str_str_index [] = {4, 2, 1, 0, 3, 6, 5, 7}; // LR preorder

// Number of entries placed in each tree.
static int RB_TREE_TEST_ENTRIES = 8;

int
main (int, ASYS_TCHAR *[])
{
  ACE_START_TEST (ASYS_TEXT ("RB_Tree_Test"));

  // @@ Chris, this function is WAY, WAY, WAY too long, which makes it
  // impossible to tell what's going on...  Please break it up into a
  // number of smaller functions.

  // Local variables used to index arrays.
  int i;
  int k;

  // Construct eight RB_Trees.  Specialization of the ACE_Less_Than
  // template for character strings performs strcmp style string
  // comparisons rather than < operator comparison of the pointers
  // themselves.

  INT_INT_RB_TREE int_int_tree1;
  INT_INT_RB_TREE int_int_tree2;
  INT_STR_RB_TREE int_str_tree1;
  INT_STR_RB_TREE int_str_tree2;
  STR_INT_RB_TREE str_int_tree1;
  STR_INT_RB_TREE str_int_tree2;
  STR_STR_RB_TREE str_str_tree1;
  STR_STR_RB_TREE str_str_tree2;

  // First, test the new ACE_Hash_Map_Manager_Ex compliant interface.
  // Fill in each tree with the key and item from the appropriate
  // arrays, using the shuffle indexes to create different insertion
  // orders.

  for (i = 0;
       i < RB_TREE_TEST_ENTRIES;
       ++i)
    {
      const char *str_item;
      int int_item;

      k = int_int_index [i];
      ACE_ASSERT (k >= 0 && k < RB_TREE_TEST_ENTRIES);
      int_item = -1;
      ACE_ASSERT (int_int_tree1.insert (number_integers [k],
                                        number_integers [k]) != 0);
      ACE_ASSERT (int_int_tree1.find (number_integers [k], int_item) == 0
                  && int_item == number_integers [k]);

      k = int_str_index [i];
      ACE_ASSERT (k >= 0 && k < RB_TREE_TEST_ENTRIES);
      str_item = 0;
      ACE_ASSERT (int_str_tree1.insert (number_integers [k],
                                        number_strings [k]) != 0);
      ACE_ASSERT (int_str_tree1.find (number_integers [k], str_item) == 0
                  && str_item == number_strings [k]);

      k = str_int_index [i];
      ACE_ASSERT (k >= 0 && k < RB_TREE_TEST_ENTRIES);
      int_item = -1;
       ACE_ASSERT (str_int_tree1.insert (number_strings [k],
                                         number_integers [k]) != 0);
      ACE_ASSERT (str_int_tree1.find (number_strings [k], int_item) == 0
                  && int_item == number_integers [k]);

      k = str_str_index [i];
      ACE_ASSERT (k >= 0 && k < RB_TREE_TEST_ENTRIES);
      str_item = 0;
       ACE_ASSERT (str_str_tree1.insert (number_strings [k],
                                         number_strings [k]) != 0);
      ACE_ASSERT (str_str_tree1.find (number_strings [k], str_item) == 0
                  && str_item == number_strings [k]);
    }


  // Second, test the deprecated interface.  This portion of the test
  // will go away when the deprecated interface is removed Fill in
  // each tree with the key and item from the appropriate arrays,
  // using the shuffle indexes to create different insertion orders.
  for (i = 0;
       i < RB_TREE_TEST_ENTRIES;
       ++i)
    {
      k = int_int_index [i];
      ACE_ASSERT (k >= 0 && k < RB_TREE_TEST_ENTRIES);
      ACE_ASSERT (int_int_tree2.insert (number_integers [k],
                                        number_integers [k]) != 0);
      ACE_ASSERT (int_int_tree2.find (number_integers [k]) != 0
                  && *int_int_tree2.find (number_integers [k]) == number_integers [k]);

      k = int_str_index [i];
      ACE_ASSERT (k >= 0 && k < RB_TREE_TEST_ENTRIES);
      ACE_ASSERT (int_str_tree2.insert (number_integers [k],
                                        number_strings [k]) != 0);
      ACE_ASSERT (int_str_tree2.find (number_integers [k]) != 0
                  && *int_str_tree2.find (number_integers [k]) ==
                      number_strings [k]);

      k = str_int_index [i];
      ACE_ASSERT (k >= 0 && k < RB_TREE_TEST_ENTRIES);
      ACE_ASSERT (str_int_tree2.insert (number_strings [k],
                                        number_integers [k]) != 0);
      ACE_ASSERT (str_int_tree2.find (number_strings [k]) != 0
                  && *str_int_tree2.find (number_strings [k]) ==
                      number_integers [k]);

      k = str_str_index [i];
      ACE_ASSERT (k >= 0 && k < RB_TREE_TEST_ENTRIES);
      ACE_ASSERT (str_str_tree2.insert (number_strings [k],
                                        number_strings [k]) != 0);
      ACE_ASSERT (str_str_tree2.find (number_strings [k]) != 0
                  && *str_str_tree2.find (number_strings [k]) ==
                     number_strings [k]);
    }


  // Construct a forward and reverse iterator for each of the trees.

  INT_INT_FWD_ITER int_int_iter1 (int_int_tree1);
  INT_STR_FWD_ITER int_str_iter1 (int_str_tree1);
  STR_INT_FWD_ITER str_int_iter1 (str_int_tree1);
  STR_STR_FWD_ITER str_str_iter1 (str_str_tree1);

  INT_INT_REV_ITER int_int_rev_iter1 (int_int_tree1);
  INT_STR_REV_ITER int_str_rev_iter1 (int_str_tree1);
  STR_INT_REV_ITER str_int_rev_iter1 (str_int_tree1);
  STR_STR_REV_ITER str_str_rev_iter1 (str_str_tree1);

  INT_INT_FWD_ITER int_int_iter2 (int_int_tree2);
  INT_STR_FWD_ITER int_str_iter2 (int_str_tree2);
  STR_INT_FWD_ITER str_int_iter2 (str_int_tree2);
  STR_STR_FWD_ITER str_str_iter2 (str_str_tree2);

  INT_INT_REV_ITER int_int_rev_iter2 (int_int_tree2);
  INT_STR_REV_ITER int_str_rev_iter2 (int_str_tree2);
  STR_INT_REV_ITER str_int_rev_iter2 (str_int_tree2);
  STR_STR_REV_ITER str_str_rev_iter2 (str_str_tree2);

  // Iterate over each of the trees, making sure their entries are in
  // the same relative order (i.e., the integers and strings represent
  // the same values at each respective position in the tree).
  for (i = 0;
       i < RB_TREE_TEST_ENTRIES;
       ++i)
    {
      const char *str_item;
      int int_item;

      int_item = (*int_int_iter1).item ();
      ACE_ASSERT (int_item == number_integers [i]);

      int_item = (*int_int_rev_iter1).item ();
      ACE_ASSERT (int_item == number_integers [RB_TREE_TEST_ENTRIES - i - 1]);

      int_item = (*str_int_iter1).item ();
      ACE_ASSERT (int_item == number_integers [i]);

      int_item = (*str_int_rev_iter1).item ();
      ACE_ASSERT (int_item == number_integers [RB_TREE_TEST_ENTRIES - i - 1]);

      str_item = (*int_str_iter1).item ();
      ACE_ASSERT (str_item == number_strings [i]);

      str_item = (*int_str_rev_iter1).item ();
      ACE_ASSERT (str_item == number_strings [RB_TREE_TEST_ENTRIES - i - 1]);

      str_item = (*str_str_iter1).item ();
      ACE_ASSERT (str_item == number_strings [i]);

      str_item = (*str_str_rev_iter1).item ();
      ACE_ASSERT (str_item == number_strings [RB_TREE_TEST_ENTRIES - i - 1]);

      int_item = (*int_int_iter2).item ();
      ACE_ASSERT (int_item == number_integers [i]);

      int_item = (*int_int_rev_iter2).item ();
      ACE_ASSERT (int_item == number_integers [RB_TREE_TEST_ENTRIES - i - 1]);

      int_item = (*str_int_iter2).item ();
      ACE_ASSERT (int_item == number_integers [i]);

      int_item = (*str_int_rev_iter2).item ();
      ACE_ASSERT (int_item == number_integers [RB_TREE_TEST_ENTRIES - i - 1]);

      str_item = (*int_str_iter2).item ();
      ACE_ASSERT (str_item == number_strings [i]);

      str_item = (*int_str_rev_iter2).item ();
      ACE_ASSERT (str_item == number_strings [RB_TREE_TEST_ENTRIES - i - 1]);

      str_item = (*str_str_iter2).item ();
      ACE_ASSERT (str_item == number_strings [i]);

      str_item = (*str_str_rev_iter2).item ();
      ACE_ASSERT (str_item == number_strings [RB_TREE_TEST_ENTRIES - i - 1]);

      // Advance each iterator.
      ++int_int_iter1;
      ++int_str_iter1;
      ++str_int_iter1;
      ++str_str_iter1;
      ++int_int_rev_iter1;
      ++int_str_rev_iter1;
      ++str_int_rev_iter1;
      ++str_str_rev_iter1;
      ++int_int_iter2;
      ++int_str_iter2;
      ++str_int_iter2;
      ++str_str_iter2;
      ++int_int_rev_iter2;
      ++int_str_rev_iter2;
      ++str_int_rev_iter2;
      ++str_str_rev_iter2;
    }

  // Make sure each item in each tree has been visited
  ACE_ASSERT (int_int_iter1.done () == 1);
  ACE_ASSERT (int_str_iter1.done () == 1);
  ACE_ASSERT (str_int_iter1.done () == 1);
  ACE_ASSERT (str_str_iter1.done () == 1);
  ACE_ASSERT (int_int_rev_iter1.done () == 1);
  ACE_ASSERT (int_str_rev_iter1.done () == 1);
  ACE_ASSERT (str_int_rev_iter1.done () == 1);
  ACE_ASSERT (str_str_rev_iter1.done () == 1);
  ACE_ASSERT (int_int_iter2.done () == 1);
  ACE_ASSERT (int_str_iter2.done () == 1);
  ACE_ASSERT (str_int_iter2.done () == 1);
  ACE_ASSERT (str_str_iter2.done () == 1);
  ACE_ASSERT (int_int_rev_iter2.done () == 1);
  ACE_ASSERT (int_str_rev_iter2.done () == 1);
  ACE_ASSERT (str_int_rev_iter2.done () == 1);
  ACE_ASSERT (str_str_rev_iter2.done () == 1);

  // Remove the even numbered entries from each of the trees.  New
  // interface.
  for (i = 0;
       i < RB_TREE_TEST_ENTRIES;
       i += 2)
    {
      ACE_ASSERT (int_int_tree1.unbind (number_integers [i]) == 0);
      ACE_ASSERT (int_str_tree1.unbind (number_integers [i]) == 0);
      ACE_ASSERT (str_int_tree1.unbind (number_strings [i]) == 0);
      ACE_ASSERT (str_str_tree1.unbind (number_strings [i]) == 0);
    }

  // Remove the even numbered entries from each of the trees.
  // Deprecated interface.
  for (i = 0;
       i < RB_TREE_TEST_ENTRIES;
       i += 2)
    {
      ACE_ASSERT (int_int_tree2.remove (number_integers [i]) == 1);
      ACE_ASSERT (int_str_tree2.remove (number_integers [i]) == 1);
      ACE_ASSERT (str_int_tree2.remove (number_strings [i]) == 1);
      ACE_ASSERT (str_str_tree2.remove (number_strings [i]) == 1);
    }

  // Reset iterators

  int_int_iter1 = int_int_tree1.begin ();
  int_str_iter1 = int_str_tree1.begin ();
  str_int_iter1 = str_int_tree1.begin ();
  str_str_iter1 = str_str_tree1.begin ();
  int_int_rev_iter1 = int_int_tree1.rbegin ();
  int_str_rev_iter1 = int_str_tree1.rbegin ();
  str_int_rev_iter1 = str_int_tree1.rbegin ();
  str_str_rev_iter1 = str_str_tree1.rbegin ();
  int_int_iter2 = int_int_tree2.begin ();
  int_str_iter2 = int_str_tree2.begin ();
  str_int_iter2 = str_int_tree2.begin ();
  str_str_iter2 = str_str_tree2.begin ();
  int_int_rev_iter2 = int_int_tree2.rbegin ();
  int_str_rev_iter2 = int_str_tree2.rbegin ();
  str_int_rev_iter2 = str_int_tree2.rbegin ();
  str_str_rev_iter2 = str_str_tree2.rbegin ();

  // Iterate over each of the trees, making sure their entries are
  // still in the same relative order (i.e., the integers and strings
  // represent the same values at each respective position in the
  // tree).
  for (i = 1;
       i < RB_TREE_TEST_ENTRIES;
       i += 2)
    {
      const char *str_item;
      int int_item;

      int_item = (*int_int_iter1).item ();
      ACE_ASSERT (int_item == number_integers [i]);

      int_item = (*int_int_rev_iter1).item ();
      ACE_ASSERT (int_item == number_integers [RB_TREE_TEST_ENTRIES - i]);

      int_item = (*str_int_iter1).item ();
      ACE_ASSERT (int_item == number_integers [i]);

      int_item = (*str_int_rev_iter1).item ();
      ACE_ASSERT (int_item == number_integers [RB_TREE_TEST_ENTRIES - i]);

      str_item = (*int_str_iter1).item ();
      ACE_ASSERT (str_item == number_strings [i]);

      str_item = (*int_str_rev_iter1).item ();
      ACE_ASSERT (str_item == number_strings [RB_TREE_TEST_ENTRIES - i]);

      str_item = (*str_str_iter1).item ();
      ACE_ASSERT (str_item == number_strings [i]);

      str_item = (*str_str_rev_iter1).item ();
      ACE_ASSERT (str_item == number_strings [RB_TREE_TEST_ENTRIES - i]);

      int_item = (*int_int_iter2).item ();
      ACE_ASSERT (int_item == number_integers [i]);

      int_item = (*int_int_rev_iter2).item ();
      ACE_ASSERT (int_item == number_integers [RB_TREE_TEST_ENTRIES - i]);

      int_item = (*str_int_iter2).item ();
      ACE_ASSERT (int_item == number_integers [i]);

      int_item = (*str_int_rev_iter2).item ();
      ACE_ASSERT (int_item == number_integers [RB_TREE_TEST_ENTRIES - i]);

      str_item = (*int_str_iter2).item ();
      ACE_ASSERT (str_item == number_strings [i]);

      str_item = (*int_str_rev_iter2).item ();
      ACE_ASSERT (str_item == number_strings [RB_TREE_TEST_ENTRIES - i]);

      str_item = (*str_str_iter2).item ();
      ACE_ASSERT (str_item == number_strings [i]);

      str_item = (*str_str_rev_iter2).item ();
      ACE_ASSERT (str_item == number_strings [RB_TREE_TEST_ENTRIES - i]);

      // Advance each iterator via postfix increment.
      int_int_iter1++;
      int_str_iter1++;
      str_int_iter1++;
      str_str_iter1++;
      int_int_rev_iter1++;
      int_str_rev_iter1++;
      str_int_rev_iter1++;
      str_str_rev_iter1++;
      int_int_iter2++;
      int_str_iter2++;
      str_int_iter2++;
      str_str_iter2++;
      int_int_rev_iter2++;
      int_str_rev_iter2++;
      str_int_rev_iter2++;
      str_str_rev_iter2++;
    }

  // Make sure each item in each tree has been visited a second time.
  ACE_ASSERT (int_int_rev_iter1.done () == 1);
  ACE_ASSERT (int_str_rev_iter1.done () == 1);
  ACE_ASSERT (str_int_rev_iter1.done () == 1);
  ACE_ASSERT (str_str_rev_iter1.done () == 1);
  ACE_ASSERT (int_int_rev_iter2.done () == 1);
  ACE_ASSERT (int_str_rev_iter2.done () == 1);
  ACE_ASSERT (str_int_rev_iter2.done () == 1);
  ACE_ASSERT (str_str_rev_iter2.done () == 1);
  ACE_ASSERT (int_int_iter1.done () == 1);
  ACE_ASSERT (int_str_iter1.done () == 1);
  ACE_ASSERT (str_int_iter1.done () == 1);
  ACE_ASSERT (str_str_iter1.done () == 1);
  ACE_ASSERT (int_int_iter2.done () == 1);
  ACE_ASSERT (int_str_iter2.done () == 1);
  ACE_ASSERT (str_int_iter2.done () == 1);
  ACE_ASSERT (str_str_iter2.done () == 1);

  ACE_END_TEST;
  return 0;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_RB_Tree<int, int, ACE_Less_Than<int>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Node<int, int>;
template class ACE_RB_Tree_Iterator_Base<int, int, ACE_Less_Than<int>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Iterator<int, int, ACE_Less_Than<int>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Reverse_Iterator<int, int, ACE_Less_Than<int>, ACE_Null_Mutex>;
template class ACE_RB_Tree<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Node<int, const char *>;
template class ACE_RB_Tree_Iterator_Base<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Iterator<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Reverse_Iterator<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex>;
template class ACE_RB_Tree<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Node<const char *, int>;
template class ACE_RB_Tree_Iterator_Base<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Iterator<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Reverse_Iterator<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex>;
template class ACE_RB_Tree<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Node<const char *, const char *>;
template class ACE_RB_Tree_Iterator_Base<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Iterator<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex>;
template class ACE_RB_Tree_Reverse_Iterator<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex>;
template class ACE_Less_Than<int>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_RB_Tree<int, int, ACE_Less_Than<int>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Node<int, int>
#pragma instantiate ACE_RB_Tree_Iterator_Base<int, int, ACE_Less_Than<int>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Iterator<int, int, ACE_Less_Than<int>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Reverse_Iterator<int, int, ACE_Less_Than<int>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Node<int, const char *>
#pragma instantiate ACE_RB_Tree_Iterator_Base<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Iterator<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Reverse_Iterator<int, const char *, ACE_Less_Than<int>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Node<const char *, int>
#pragma instantiate ACE_RB_Tree_Iterator_Base<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Iterator<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Reverse_Iterator<const char *, int, ACE_Less_Than<const char *>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Node<const char *, const char *>
#pragma instantiate ACE_RB_Tree_Iterator_Base<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Iterator<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex>
#pragma instantiate ACE_RB_Tree_Reverse_Iterator<const char *, const char *, ACE_Less_Than<const char *>, ACE_Null_Mutex>
#pragma instantiate ACE_Less_Than<int>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
