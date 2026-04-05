#include "server_utils.h"
#include "path_utils.h"
#include "search_utils.h"
#include <vector>

seeddata smdata,usumdata;

crow::response handleSeed(const crow::request& req, bool isUltra, bool isId) {
    auto needle = req.url_params.get("needle");
    if (needle == nullptr) {
        return crow::response(400);
    }
    std::vector<uint32_t> clocks;
    parseLine(needle, clocks);
    uint32_t skipped = isUltra ? 477 : 417;
    if(isId) {
        skipped = isUltra ? 1132 : 1012;
    }

    
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
    
    // CROW_ROUTE(app, "/<string>/sfmt/seed/id")
    // ([](const crow::request& req, std::string mode){
    //     if (mode != "usm" && mode != "sm")
    //         return crow::response(404);
        
    //     return handleSeed(req, mode == "usm", true);
    // });
}

int main(int argc, char* argv[]) {
    smdata.loadfiles(417);
    usumdata.loadfiles(477);
    
    crow::SimpleApp app;
    make_routes(app);
    
    app.port(PORT).run();
    
    smdata.close();
    usumdata.close();
}