@0x8b03f59616e377c8;

interface Synchro {
    struct Direction {
        accelerate @0 :Bool;
        decelerate @1 :Bool;
        rotateLeft @2 :Bool;
        rotateRight @3 :Bool;
    }

    connect @0 () -> ();
}
