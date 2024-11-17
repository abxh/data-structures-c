/*
    Pool struct should be opaque if the library is made non-header-only. So the pool's members are not tested.

    Mutating operation types:
    - init
    - deallocate_all
    - allocate
    - deallocate

    Branches:
    - allocate()
        | no available nodes -> NULL
        | otherwise -> pointer to chunk of memory the pool initialized for
*/

int main() {

}
