@0x8b03f59616e377c8;

interface Backend {
# The thing running as backend, synchronising registered spaceships across servers

    struct Direction {
    # Directional commands for the spaceship
        accelerate  @0 :Int8 = -1;
        decelerate  @1 :Int8 = -1;
        rotateLeft  @2 :Int8 = -1;
        rotateRight @3 :Int8 = -1;
    }

    struct Item {
    # A wrapper for everything to be sent. The contents of this may be adjusted to the game as needed
        timestamp @0 :Int32;
        spaceship @1 :Spaceship;
        direction @2 :Direction;
    }

    struct Spaceship {
    # Everything needed to perfectly reconstruct a spaceship. Used to synchronise local and remote ships
        username @0 :Text;
        position @1 :Vector;
        velocity @2 :Vector;
        angle    @3 :Float32;
        health   @4 :Int8;
    }

    struct Vector {
    # Two-dimensional vector with two values referring to the two axes, respectively
        x @0 :Float32;
        y @1 :Float32;
    }

    interface ShipSink {
    # Input of the stream used to update ships

        done @0 ();
        # Tell the sink that the ship is done and will be destroyed

        sendItem @1 (item :Item);
        # Update the corresponding ship(s)

        getShip @2 () -> (ship :Spaceship);
        # A callback for getting this ship's current status. Needed for remote ships for which no ShipHandle exists
    }

    interface ShipHandle {
    # A handle for a ship through which it can be monitored and controlled (and checked for life signs)
        getSink @0 () -> (sink :ShipSink);
        getShip @1 () -> (ship :Spaceship);
        setShip @2 (ship :Spaceship) -> ();
        ping @3 ();
    }

    interface Registrar {
    # Callback distributed to backends and clients. Used to introduce and distribute new ships, respectively
        registerShip @0 (ship :Spaceship, handle :ShipHandle) -> (sink :ShipSink);
    }

    interface Synchro {
    # The thing that synchronises ships across connected instances
        connect @0 (id :Text, synchro :Synchro, registrar :Registrar) -> (id :Text, registrar :Registrar);
        share   @1 (id :Text, synchro :Synchro);
    }

    ping @0 ();
    # Empty request to check whether server is running

    seed @1 () -> (seed :UInt64);
    # Request RNG seed server is running for

    connect @2 (id :Text, registrar :Registrar) -> (id :Text, registrar :Registrar, synchro :Synchro);
    # Subscribe local game client to backend, exchanging registration handles in the process

    join @3 (id :Text, remote :Synchro);
    # Tell a remote backend of our local synchro
}
