// Adapted from http://people.cs.ksu.edu/~sherrill/labs/lab05/lineage.cpp
#include "person.h"
#include "personList.h"

int main() {
    PersonList theList;

    theList.addPerson("Bob", "Mark", "Betty");
    theList.addPerson("Jim", "Bob", "Sally");
    theList.addPerson("Frank", "Jim", "Mary");
    theList.addPerson("Leonard", "Jim", "Mary");
    theList.addPerson("Kim", "Leonard", "Sarah");

    theList.printLineage("Jim");
    theList.printLineage("Kim");
    theList.printLineage("Betty");

    return 0;
} 

/* Output (your revised solution should give the same output)
Ancestors of Jim
mother: Sally
father: Bob
grand mother: Betty
grand father: Mark

Decendents of Jim
child: Frank
child: Leonard
grand child: Kim

Ancestors of Kim
mother: Sarah
father: Leonard
grand mother: Mary
grand father: Jim
great grand mother: Sally
great grand father: Bob
great great grand mother: Betty
great great grand father: Mark

Decendents of Kim

Ancestors of Betty

Decendents of Betty
child: Bob
grand child: Jim
great grand child: Frank
great grand child: Leonard
great great grand child: Kim
*/
