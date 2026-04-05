#include "server_utils.h"
#include "path_utils.h"
#include "search_utils.h"

seeddata smmain, usummain, smid, usumid;

crow::response handleSeed(const crow::request& req, bool isUltra, bool isId) {
    auto needle = req.url_params.get("needle");
    if (needle == nullptr) {
        return crow::response(400);
    }
    std::string needlestr(needle);
    std::vector<uint32_t> clocks;
    parseLine(needlestr, clocks);
    rotateClocks(clocks, isId);
    uint32_t skipped = isUltra ? 477 : 417;
    if(isId) {
        skipped = isUltra ? 1132 : 1012;
    }
    seeddata data;
    if(isId) {
        data = isUltra ? usumid : smid;
    } else {
        data = isUltra ? usummain : smmain;
    }
    std::set<uint32_t> seeds;
    size_t count = search_seeds(clocks, skipped, data, seeds);
    if(count > 100) {
        return crow::response(400);
    }
    
    return crow::response(200); // figure out the response format later
}

void make_routes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/ping")([](){
        return "pong!";
    });

    CROW_ROUTE(app, "/<string>/sfmt/seed")
    ([](const crow::request& req, std::string mode){
        if (mode != "usm" && mode != "sm")
            return crow::response(404);
        
        return handleSeed(req, mode == "usm", false); 
    });
    
    CROW_ROUTE(app, "/<string>/sfmt/seed/id")
    ([](const crow::request& req, std::string mode){
        if (mode != "usm" && mode != "sm")
            return crow::response(404);
        
        return handleSeed(req, mode == "usm", true);
    });
}

int main(int argc, char* argv[]) {
    smmain.loadfiles(417);
    usummain.loadfiles(477);
    smid.loadfiles(417);
    usumid.loadfiles(477);
    
    crow::SimpleApp app;
    make_routes(app);
    
    app.port(PORT).run();
    
    smmain.close();
    usummain.close();
    smid.close();
    usumid.close();
}