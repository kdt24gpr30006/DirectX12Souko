#pragma once
#include <unordered_map>
#include <memory>
#include <string>

class Texture;

class TextureManager
{
public:

    static TextureManager& Instance();

    // Textureの読み込み
    Texture* LoadTexture(const std::string& path);

    // ゲーム終了時に解放
    void Clear();

private:

    TextureManager() = default;
    ~TextureManager() = default;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

private:
    // テクスチャ格納用
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
};
