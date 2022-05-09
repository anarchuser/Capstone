@0x8b03f59616e377c8;

interface Synchro {
    struct Direction {
        accelerate  @0 :Int8 = -1;
        decelerate  @1 :Int8 = -1;
        rotateLeft  @2 :Int8 = -1;
        rotateRight @3 :Int8 = -1;
    }

    struct State {
        nothing @0 :Void;
    }

    struct Position {
        x @0 :Float32;
        y @1 :Float32;
    }

    interface Callback {
        disconnect   @0 ();
        requestItem  @1 () -> (item :Direction);
        requestState @2 () -> (state :State);
        updateState  @3 (state :State);
    }

    struct Maybe(T) {
        union {
            nothing @0 :Void;
            value   @1 :T;
        }
    }

    ping        @0 ();
    randomSeed  @1 () -> (seed :UInt64);
    join        @2 (other :Maybe(Synchro), callback :Callback);
}
