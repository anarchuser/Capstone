@0x8b03f59616e377c8;

interface Synchro {
    struct Direction {
        accelerate  @0 :Int8 = -1;
        decelerate  @1 :Int8 = -1;
        rotateLeft  @2 :Int8 = -1;
        rotateRight @3 :Int8 = -1;
    }

    struct Position {
        x @0 :Float32;
        y @1 :Float32;
    }

    interface Callback(T, U) {
        requestItem  @0 (item :T)  -> stream;
        requestState @1 (state :U) -> (state :U);
        done         @2 ();
    }

    struct Maybe(T) {
        union {
            nothing @0 :Void;
            value   @1 :T;
        }
    }

    struct Nullptr {
        nothing @0 :Void;
    }

    ping        @0 ();
    randomSeed  @1 () -> (seed :UInt64);
    join        @2 (other :Maybe(Synchro), callback :Callback(Direction, Nullptr));
}
