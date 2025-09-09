#include "apiclient.h"
#include <iostream>
#include <curl/curl.h>

// HTTP Response Buffer
struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    auto* mem = (MemoryStruct*)userp;
    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

json APIClient::DetectSeams(const fs::path& image_path) {
    CURL* curl;
    CURLcode res;
    MemoryStruct chunk;
    chunk.memory = (char*)malloc(1);
    chunk.size = 0;

    curl = curl_easy_init();
    if(curl) {
        curl_mime* form = curl_mime_init(curl);

        // Attach file
        curl_mimepart* field = curl_mime_addpart(form);
        curl_mime_name(field, "file");
        curl_mime_filedata(field, image_path.string().c_str());

        // Metadata
        json metadata = {{"image_id", image_path.stem().string()}};
        field = curl_mime_addpart(form);
        curl_mime_name(field, "metadata");
        curl_mime_data(field, metadata.dump().c_str(), CURL_ZERO_TERMINATED);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("X-API-Key: " + api_key_).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, api_url_.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_mime_free(form);
        curl_slist_free_all(headers);

        if(res != CURLE_OK) {
            std::cerr << "API request failed: " << curl_easy_strerror(res) << std::endl;
            return {{"error", "API request failed"}};
        } else {
            std::cout << "Raw JSON response:" << std::endl;
            std::cout << chunk.memory << std::endl;
            return json::parse(chunk.memory);
        }
    }
    return {{"error", "CURL init failed"}};
}

json APIClient::DetectSeams(const cv::Mat& image, const std::string& id_dummy) {
    CURL* curl;
    CURLcode res;
    MemoryStruct chunk;
    chunk.memory = (char*)malloc(1);
    chunk.size = 0;

    // PNG encoding
    std::vector<uchar> buf;
    cv::imencode(".png", image, buf);


    curl = curl_easy_init();
    if(curl) {
        curl_mime* form = curl_mime_init(curl);

        // Add image buffer as multipart/form-data
        curl_mimepart* field = curl_mime_addpart(form);
        curl_mime_name(field, "file");
        curl_mime_data(field, reinterpret_cast<const char*>(buf.data()), buf.size());
        // Set filename (?)
        curl_mime_filename(field, (id_dummy + ".jpg").c_str());

        // Add Metadata
        json metadata = {{"image_id", id_dummy}};
        field = curl_mime_addpart(form);
        curl_mime_name(field, "metadata");
        curl_mime_data(field, metadata.dump().c_str(), CURL_ZERO_TERMINATED);

        // Add header
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("X-API-Key: " + api_key_).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, api_url_.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        // 5. 요청 수행
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_mime_free(form);
        curl_slist_free_all(headers);

        if(res != CURLE_OK) {
            std::cerr << "API request failed: " << curl_easy_strerror(res) << std::endl;
            return {{"error", "API request failed"}};
        } else {
            std::cout << "Raw JSON response:" << std::endl;
            std::cout << chunk.memory << std::endl;
            return json::parse(chunk.memory);
        }
    }

    return {{"error", "CURL init failed"}};
}
