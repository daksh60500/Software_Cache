#include "cache_client.hpp"
#include <stdio.h>
#include <random>
#include <vector>
#include <chrono>
#include <fstream>
#include <cstdlib>

typedef std::ratio<1l, 1000000l> micro;
typedef std::chrono::duration<long long, micro> microseconds;
#define timeNow() std::chrono::high_resolution_clock::now()

struct time_data{
    std::vector<double> time;
    double hits;
};

struct lat_data{
    double latency;
    double req_ps;
    double hits;
};

std::vector<std::string> generate_pool(int num, int len) {
   std::vector<std::string> pool;
   std::random_device random_device;
   std::mt19937 generator(random_device());
   std::uniform_int_distribution<> distribution(0, 25);
   for (int j=0; j<num; j++) {
        std::string key = "/";
        for(int i = 0; i < len; i++)
            key += (char)(distribution(generator) + 97);
        pool.push_back(key);
   }
   return pool;
}

http::verb generate_verb(int a=63, int b=27, int c=10) {
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, a+b+c);
    int x = distribution(generator);
    if (x < a) {//these are the ranges
        return http::verb::get;
    } else if (x < a+b) {
        return http::verb::put;
    } else {
        return http::verb::delete_;
    }
}

void warm_up(Client* x, std::vector<std::string> pool, double ratio) {
    std::random_device random_device;
    std::mt19937 generator(random_device()); //a pseudo-random generator with 32 bits
    std::uniform_int_distribution<> str_distribution(0, (pool.size() * ratio)-1);
    http::request<http::empty_body> req;
    req.method(http::verb::put);
    for (int i=0; i< (int) (pool.size() * ratio); i++) {
        req.target(pool[i] + pool[str_distribution(generator)]);
        x->process(req);
    }
}

time_data baseline_latencies(int pool_size, double pool_ratio, int key_length, std::string host, std::string port) {
    std::vector<double> times;
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> str_distribution(0, pool_size -1);
    std::vector<std::string> pool = generate_pool(pool_size, key_length);
    int hits = 0;
    int total = 0;
    Client* x = new Client(host, port);
    warm_up(x, pool, pool_ratio);
    http::request<http::empty_body> req;
    for (int i=0; i<pool_size; i++) {
        req.method(generate_verb());
        switch (req.method()) {
            case http::verb::get: //
                req.target(pool[str_distribution(generator)]);
                break;
            case http::verb::put: //
                req.target(pool[str_distribution(generator)] + pool[str_distribution(generator)]);
                break;
            case http::verb::delete_://
                req.target(pool[str_distribution(generator)]);
                break;
            default:
                std::exit(EXIT_FAILURE);
        }
        auto t1 = timeNow();
        bool success = x->process(req);
        auto t2 = timeNow();
        double duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
        times.push_back(duration / 1000);
        if (success) // don't want this to be timed
            hits += 1;
        total+= 1;
    }
    time_data vals;
    vals.time = times;
    vals.hits = (double) hits / (double) total;
    return vals;
}


lat_data baseline_performance(int pool_size, double pool_ratio, int key_length, std::string host, std::string port){
    time_data val = baseline_latencies(pool_size, pool_ratio, key_length, host, port);
    std::vector<double> times = val.time;
    double total_time = accumulate(times.begin(),times.end(),0.0);
    lat_data result;
    result.req_ps = times.size() / total_time;
    std::sort (times.begin(),times.end());
    result.latency = times[(int) (times.size() * 0.95)];
    result.hits = val.hits;
    return result;
}

void plot(int pool_size, double pool_ratio, int key_length, std::string host, std::string port) {
    time_data vals = baseline_latencies(pool_size, pool_ratio, key_length, host, port);
    std::vector<double> times = vals.time;
    std::ofstream myfile;
    myfile.open ("baseline_latencies.csv");
    myfile << "request, time (ms)\n";
    for(int i=0; i< (int) times.size(); i++){
        myfile << i << "," << times[i] << "\n";
    }
    myfile.close();
}

void basic_test(std::string host, std::string port){
    Client x(host,port);
    http::request<http::empty_body> req;
    req.method(http::verb::get);
    req.target("/key");
    assert(x.process(req) == false);
    req.method(http::verb::get);
    req.target("/key");
    assert(x.process(req) == false);
    req.method(http::verb::put);
    req.target("/key/10");
    assert(x.process(req) == true);
    req.method(http::verb::get);
    req.target("/key");
    assert(x.process(req) == true);
    req.method(http::verb::delete_);
    req.target("/key");
    assert(x.process(req) == true);
}

void baseline_test(int pool_size, double pool_ratio, int key_length, std::string host, std::string port){
    lat_data result = baseline_performance(pool_size, pool_ratio, key_length, host, port);
    std::cout << result.req_ps << " req/sec | " << result.latency << "ms latency | "\
    << result.hits * 100 << "% hits\n";
}

int main(int argc, char* argv[]) {
    std::string port = "4000";
    std::string host = "127.0.0.1";
    int pool_size = 1024;
    int key_length = 5;
    double pool_ratio = 0.8;
    int opt;
    while((opt = getopt(argc, argv, "p:s:m:r:k:bgh")) != -1) {
        switch(opt) {
            case 'p':
                port = optarg;
                break;
            case 's':
                host = optarg;
                break;
            case 'm':
                pool_size = atoi(optarg);
                break;
            case 'r':
                pool_ratio = strtod(optarg, nullptr);
                break;
            case 'k':
                key_length = atoi(optarg);
                break;
            case 'b':
                basic_test(host, port);
                baseline_test(pool_size, pool_ratio, key_length, host, port);
                break;
            case 'g':
                plot(pool_size, pool_ratio, key_length, host, port);
                break;
            case 'h':
                std::cout << "-p : port\n-s : host\n-m : pool size\n-r : pool ratio\n-k : key length\n-b : baseline performance\n-g : graph\n-h : help\n";
                return 1;
        }
    }
}
