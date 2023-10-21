#include "CollisionSystem.h"

CollisionSystem::CollisionSystem(SDL_Renderer* renderer) : m_renderer(renderer) {}

void CollisionSystem::Update(ECSManager& ecs)
{
    std::set<int> entitiesToDestroy;

    // Syncs entity render and aabb
    for (int e = 1; e <= max_entity; e++)
    {
        if (ecs.collisionComponents.contains(e) && ecs.renderComponents.contains(e))
        {
            const SDL_Rect& dstRect = ecs.renderComponents[e].dstRect;
            ecs.collisionComponents[e].aabb = {
                static_cast<float>(dstRect.x),
                static_cast<float>(dstRect.y),
                static_cast<float>(dstRect.w),
                static_cast<float>(dstRect.h)
            };
        }
    }

    for (int e1 = 1; e1 <= max_entity; e1++)
    {
        if (ecs.collisionComponents.contains(e1) && ecs.renderComponents.contains(e1))
        {
            const AABB& box1 = ecs.collisionComponents[e1].aabb;

            for (int e2 = e1 + 1; e2 <= max_entity; e2++)
            {
                if (ecs.collisionComponents.contains(e2) && ecs.renderComponents.contains(e2))
                {
                    const AABB& box2 = ecs.collisionComponents[e2].aabb;

                    if (AABBcollision(box1, box2))
                    {
                        // If AABBs are colliding, check for per-pixel collision
                        SDL_Texture* texture1 = ecs.renderComponents[e1].texture;
                        SDL_Texture* texture2 = ecs.renderComponents[e2].texture;

                        SDL_Rect rect1 = { static_cast<int>(box1.x), static_cast<int>(box1.y), static_cast<int>(box1.width), static_cast<int>(box1.height) };
                        SDL_Rect rect2 = { static_cast<int>(box2.x), static_cast<int>(box2.y), static_cast<int>(box2.width), static_cast<int>(box2.height) };

                        if (PixelCollision(texture1, rect1, texture2, rect2))
                        {
                            entitiesToDestroy.insert(e1);
                            entitiesToDestroy.insert(e2);
                        }
                    }
                }
            }
        }
    }

    for (int entityId : entitiesToDestroy)
    {
        ecs.DestroyEntity(entityId);
    }
}

bool CollisionSystem::AABBcollision(const AABB& a, const AABB& b)
{
    return (a.x < (b.x + b.width) && (a.x + a.width) > b.x && a.y < (b.y + b.height) && (a.y + a.height) > b.y);
}

bool CollisionSystem::PixelCollision(SDL_Texture* texture1, SDL_Rect rect1, SDL_Texture* texture2, SDL_Rect rect2)
{    
    int width1, height1, width2, height2;
    SDL_QueryTexture(texture1, NULL, NULL, &width1, &height1);
    SDL_QueryTexture(texture2, NULL, NULL, &width2, &height2);

    int top = std::max(rect1.y, rect2.y);
    int bottom = std::min(rect1.y + height1, rect2.y + height2);
    int left = std::max(rect1.x, rect2.x);
    int right = std::min(rect1.x + width1, rect2.x + width2);

    for (int y = top; y < bottom; y++)
    {
        for (int x = left; x < right; x++)
        {
            if (PixelIsSolid(texture1, x - rect1.x, y - rect1.y) && PixelIsSolid(texture2, x - rect2.x, y - rect2.y))
            {
                return true;
            }
        }
    }
    return false;
}

bool CollisionSystem::PixelIsSolid(SDL_Texture* texture, int x, int y)
{
    Uint32 pixel;
    Uint32* pixels = new Uint32[1];
    SDL_Rect r = { x, y, 1, 1 };

    SDL_RenderReadPixels(m_renderer, &r, SDL_PIXELFORMAT_RGBA8888, pixels, sizeof(Uint32)); // uses sizeof(Uint32) for the pitch parameter
    pixel = pixels[0];
    delete[] pixels;

    SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888); // allocates an SDL_PixelFormat structure

    Uint8 red, green, blue, alpha;
    SDL_GetRGBA(pixel, format, &red, &green, &blue, &alpha);

    SDL_FreeFormat(format); // frees the allocated SDL_PixelFormat

    return alpha > 230;
}