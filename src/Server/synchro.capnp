@0x8b03f59616e377c8;

interface Synchro {
    struct Direction {
        accelerate  @0 :Int32 = -1;
        decelerate  @1 :Int32 = -1;
        rotateLeft  @2 :Int32 = -1;
        rotateRight @3 :Int32 = -1;
    }

    connect @0 () -> ();
    updateDirection @1 (direction :Direction) -> ();
}
