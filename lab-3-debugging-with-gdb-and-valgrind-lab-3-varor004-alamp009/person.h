#ifndef __PERSON_H__
#define __PERSON_H__

class Person{
    private:
        char *name;
        Person *father; // pointer to the father
        Person *mother; // pointer to the mother
        Person **children; // array of pointers to the kids
        int numChildren; // number of kids in children array
        int capacity; // capacity of children array

    public:
        Person(const char *name, Person* father, Person* mother);
        ~Person();

        const char* getName(){return name;}

        void addChild(Person *newChild);

        void printAncestors();
        void printDecendents();

    private:
        void printLineage(char dir, int level);
        /* helper function to compute the lineage
         * if level = 0 then returns the empty string
         * if level >= 1 then returns ("great ")^(level - 1) + "grand "
         */
        char* compute_relation(int level);
};
/* non-member function which doubles the size of t
 * NOTE: t's type will be a pointer to an array of pointers
 */
void expand(Person ***t, int *MAX);

#endif // __PERSON_H__
