#include "server_utils.h"
#include "path_utils.h"
#include "search_utils.h"
#include "log_utils.h"

#include <format>
seeddata smmain, usummain, smid, usumid;

crow::response makeSeedResp(size_t add, std::set<uint32_t> &seeds, uint32_t skipped) {
    crow::json::wvalue res;
    std::vector<crow::json::wvalue> data;
    for(uint32_t seed : seeds) {
        crow::json::wvalue item;
        item["seed"] = std::format("{:08x}",seed);
        item["step"] = skipped;
        data.push_back(item);
    }
    res["results"] = crow::json::wvalue::list(data);
    return crow::response(res);
}

crow::response handleSeed(const crow::request& req, bool isUltra, bool isId) {
    auto needle = req.url_params.get("needle");
    if (needle == nullptr) {
        return crow::response(400);
    }
    auto fuzzy = req.url_params.get("fuzzy");
    if (fuzzy != nullptr) {
        LOG(INFO, "is fuzzy");        
    }
    bool isFuzzy = fuzzy != nullptr;

    std::string needlestr(needle);
    LOG(INFO, "%s", needlestr.c_str());
    std::vector<uint32_t> clocks;
    parseLine(needlestr, clocks);
    size_t add = isId ? 15 : 0;
    rotateClocks(clocks, add);
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
    size_t count = search_seeds(clocks, skipped, data, seeds, isFuzzy);
    if(count > 10) {
        LOG(INFO, "%ld resulting seeds", count);
        return crow::response(403, std::format("{:} resulting seeds", count));
    } else {
        for(uint32_t seed : seeds) {
            LOG(INFO, "Candidate seed %08X", seed);
        }
    }

    return makeSeedResp(add, seeds, skipped);
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
    smid.loadfiles(1012);
    usumid.loadfiles(1132);
    
    crow::SimpleApp app;
    make_routes(app);
    
    app.port(PORT).run();
    
    smmain.close();
    usummain.close();
    smid.close();
    usumid.close();
}