@0x8b03f59616e377c8;

interface Backend {
# The thing running as backend, synchronising registered spaceships across servers

    struct Maybe(T) {
        union {
            nothing @0 :Void;
            value   @1 :T;
        }
    }

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

    struct Address {
        ip   @0 :Text;
        port @1:Int16;
    }

    interface Synchro {
    # The thing exchanged between servers as communication basis

        disconnect @0 ();
        sendSink @1 (sink :ItemSink);
    }

    interface ItemSink {
    # An item stream used to update entities in the game

        done @0 ();
        # Stop stream -> delete related object

        sendItem @1 (item :Item) -> ();
        # Send the next item for the related object to process
    }

    interface ShipRegistrar {
    # A registrar to tell the game client that a new spaceship has been registered

        register @0 (spaceship :Spaceship, handle :ShipHandle) -> (sink :ItemSink);
        # Tell the game client of the new spaceship
    }

    interface ShipHandle {
    # A handle for a connected spaceship giving the backend control over it

        getSpaceship @0 () -> (spaceship :Spaceship);
        setSpaceship @1 (spaceship :Spaceship) -> ();
        destroy @2 ();
    }

    ping @0 ();
    # Empty request to check whether server is running

    seed @1 () -> (seed :UInt64);
    # Request RNG seed server is running for

    newShip @2 (registrar :ShipRegistrar);
    # Register Spaceship to be managed by this server

    connect @3 (address :Address, this :Synchro) -> (their :Synchro);
    # Connect to a remote server

    requestConnect @4 (address :Address);
    # Request connection to given address
}