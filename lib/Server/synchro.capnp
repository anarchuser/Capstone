@0x8b03f59616e377c8;

interface Synchro {
    struct Direction {
        accelerate  @0 :Int8 = -1;
        decelerate  @1 :Int8 = -1;
        rotateLeft  @2 :Int8 = -1;
        rotateRight @3 :Int8 = -1;
    }

    interface DirectionCallback {
        sendDirection @0 (direction :Direction) -> stream;
        done @1 ();
    }

    connect @0 () -> ();
    updateDirection @1 (direction :Direction) -> ();
    streamDirections @2 () -> (callback :DirectionCallback);
}
