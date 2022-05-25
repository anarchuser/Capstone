@0x8b03f59616e377c8;

interface Backend {
# The thing running as backend, synchronising registered spaceships across servers

    struct Direction {
        accelerate  @0 :Int8 = -1;
        decelerate  @1 :Int8 = -1;
        rotateLeft  @2 :Int8 = -1;
        rotateRight @3 :Int8 = -1;
    }

    struct Item {
        direction @0 :Direction;
    }

    struct Spaceship {
        username @0 :Text;
        position @1 :Vector;
        velocity @2 :Vector;
        angle    @3 :Float32;
        health   @4 :Int8;
    }

    struct Vector {
        x @0 :Float32;
        y @1 :Float32;
    }

    interface Subscriber {
    }

    interface Synchro {
    }

    ping @0 ();
    # Empty request to check whether server is running

    seed @1 () -> (seed :UInt64);
    # Request RNG seed server is running for

    connect @2 (client :Subscriber) -> (synchro :Synchro);
    # Subscribe to Backend, get a local synchro instance

    join @3 (synchro :Synchro);
    # Tell a remote backend of our local synchro
}
