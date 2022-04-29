#ifndef DATABASE_SERVER_H
#define DATABASE_SERVER_H

#include "../DBMS/DBMS.h"
#include "Wrapper/Wrapper.h"

#include "../Time/Time.h"

#include "generated/ServerDBMS.capnp.h"
#include <kj/debug.h>
#include <kj/memory.h>
#include <capnp/ez-rpc.h>
#include <capnp/message.h>

/* Following Cap'n Proto Server example:
 * https://github.com/capnproto/capnproto/blob/master/c%2B%2B/samples/calculator-server.c%2B%2B
 */

class DatabaseImpl final : public RPCServer::Server {
public:
    kj::Promise <void> connect (ConnectContext context) override {
        log ("Listening");
        return kj::READY_NOW;
    }

    kj::Promise <void> create (CreateContext context) override {
        log ("Create request", context);
        Target target = Wrapper::unwrap (context.getParams().getTarget());

        LOG_TIME (DBMS::create (target));

        return kj::READY_NOW;
    }

    kj::Promise <void> select (SelectContext context) override {
        log ("Select request", context);
        Target target = Wrapper::unwrap (context.getParams().getTarget());
        std::unique_ptr <Table> selection;

        LOG_TIME (selection = std::make_unique<Table>(DBMS::select (target)));

        kj::Own <capnp::MallocMessageBuilder> builder = std::move (Wrapper::wrap (* selection));
        context.getResults().setData (builder->template getRoot <::RPCServer::Table>().asReader());
        return kj::READY_NOW;
    }

    kj::Promise <void> modify (ModifyContext context) override {
        log ("Modify request", context);
        qy::Row row = Wrapper::unwrap (context.getParams().getTarget());
        std::vector <Cell> data = Wrapper::unwrap (context.getParams().getTarget().getData());

        LOG_TIME (DBMS::modify (row));

        return kj::READY_NOW;
    }

    kj::Promise <void> insert (InsertContext context) override {
        log ("Insert request", context);
        qy::Row row = Wrapper::unwrap (context.getParams().getTarget());

        LOG_TIME (DBMS::insert (row));

        return kj::READY_NOW;
    }

    kj::Promise <void> remove (RemoveContext context) override {
        log ("Remove request", context);
        Target target = Wrapper::unwrap (context.getParams().getTarget());

        LOG_TIME (DBMS::remove(target));

        return kj::READY_NOW;
    }


    kj::Promise <void> sendQuery (auto context){
        std::cout << "Server: 'Received Query: " << context.getParams ().getQuery() << "'" << std::endl;
        kj::Own <capnp::MallocMessageBuilder> responseBuilder = std::move (Wrapper::wrap(evalQuery (context.getParams().getQuery())));
        context.getResults().setResponse (responseBuilder->template getRoot <RPCServer::Maybe<RPCServer::Table>>().asReader());
        return kj::READY_NOW;
    }

private:
    void log (std::string msg) {
        LOG (INFO) << "Server @" << this << ": '" << msg << "'";
    }
    template <class T>
    void log (std::string msg, T context) {
        log (msg);
        log (context.getParams().toString().flatten().cStr());
    }
};

#endif //DATABASE_SERVER_H

/* Copyright (C) 2020 Aaron Alef & Felix Bachstein */