#include "TextureManager.h"
#include <cassert>
#include <string>
#include <Graphics/Texture/Texture.h>
#include <memory>
#include <utility>

TextureManager& TextureManager::Instance()
{
    static TextureManager instance;
    return instance;
}

Texture* TextureManager::LoadTexture(const std::string& path)
{
    // ‚·‚Å‚É“Ç‚İ‚Ü‚ê‚Ä‚¢‚é‚©ƒ`ƒFƒbƒN
    auto it = mTextures.find(path);
    if (it != mTextures.end())
    {
        return it->second.get();
    }

    // “Ç‚İ‚İ
    auto texture = std::make_unique<Texture>();
    bool result = texture->Create(path);

    // “Ç‚İ‚İ¸”s‚Ìê‡~‚ß‚é
    assert(result);

    // ƒ}ƒbƒv‚É“o˜^
    Texture* ptr = texture.get();
    mTextures.emplace(path, std::move(texture));
    return ptr;
}

void TextureManager::Clear()
{
    mTextures.clear();
}
