/*
MIT License

Copyright (c) 2018 LAK132

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <vector>
#include <map>
#include <string>
#include <tuple>
#include <variant>
#include <type_traits>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <gl3w.h>

#include "types/json.h"

#ifndef LAK_GLTF_H
#define LAK_GLTF_H

namespace lak
{
    using std::vector;
    using std::map;
    using std::string;
    using std::unordered_map;
    using std::variant;
    using namespace std::string_literals;

    struct gltf_t
    {
        struct accessor_t
        {
            struct sparse_t
            {
                struct indices_t
                {
                    size_t bufferView;
                    size_t byteOffset;
                    GLenum componentType;
                    template<typename ...JT>
                    bool operator=(const json_t<JT...> &json)
                    {
                        bool rtn = false;
                        rtn |= json("bufferView"s, bufferView);
                        rtn |= json("byteOffset"s, byteOffset);
                        rtn |= json("componentType"s, componentType);
                        return rtn;
                    }
                    template<typename ...JT>
                    friend inline void operator<<(json_t<JT...> &json, const indices_t &indices)
                    {
                        json = json_object_t<JT...>{};
                        json["bufferView"s] = indices.bufferView;
                        json["byteOffset"s] = indices.byteOffset;
                        json["componentType"s] = indices.componentType;
                    }
                };
                struct values_t
                {
                    size_t bufferView;
                    size_t byteOffset;
                    template<typename ...JT>
                    bool operator=(const json_t<JT...> &json)
                    {
                        bool rtn = false;
                        rtn |= json("bufferView"s, bufferView);
                        rtn |= json("byteOffset"s, byteOffset);
                        return rtn;
                    }
                    template<typename ...JT>
                    friend inline void operator<<(json_t<JT...> &json, const values_t &values)
                    {
                        json = json_object_t<JT...>{};
                        json["bufferView"s] = values.bufferView;
                        json["byteOffset"s] = values.byteOffset;
                    }
                };
                size_t count;
                indices_t indices;
                values_t values;
                template<typename ...JT>
                bool operator=(const json_t<JT...> &json)
                {
                    bool rtn = false;
                    rtn |= json("count"s, count);
                    rtn |= json("indices"s, indices);
                    rtn |= json("values"s, values);
                    return rtn;
                }
                template<typename ...JT>
                friend inline void operator<<(json_t<JT...> &json, const sparse_t &sparce)
                {
                    json = json_object_t<JT...>{};
                    json["count"s] = sparce.count;
                    json["indices"s] = sparce.indices;
                    json["values"s] = sparce.values;
                }
            };
            size_t bufferView;
            size_t byteOffset;
            size_t count;
            GLenum componentType;
            string type;
            vector<double> min;
            vector<double> max;
            sparse_t sparse;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("bufferView"s, bufferView);
                rtn |= json("byteOffset"s, byteOffset);
                rtn |= json("count"s, count);
                rtn |= json("componentType"s, componentType);
                rtn |= json("type"s, type);
                rtn |= json("min"s, min);
                rtn |= json("max"s, max);
                rtn |= json("sparse"s, sparse);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const accessor_t &accessor)
            {
                json = json_object_t<JT...>{};
                json["bufferView"s] = accessor.bufferView;
                json["byteOffset"s] = accessor.byteOffset;
                json["count"s] = accessor.count;
                json["componentType"s] = accessor.componentType;
                json["type"s] = accessor.type;
                json["min"s] = accessor.min;
                json["max"s] = accessor.max;
                json["sparse"s] = accessor.sparse;
            }
        };

        struct animation_t
        {
            struct channel_t
            {
                struct target_t
                {
                    size_t node;
                    string path;
                    template<typename ...JT>
                    bool operator=(const json_t<JT...> &json)
                    {
                        bool rtn = false;
                        rtn |= json("node"s, node);
                        rtn |= json("path"s, path);
                        return rtn;
                    }
                    template<typename ...JT>
                    friend inline void operator<<(json_t<JT...> &json, const target_t &target)
                    {
                        json = json_object_t<JT...>{};
                        json["node"s] = target.node;
                        json["path"s] = target.path;
                    }
                };
                size_t sampler;
                target_t target;
                template<typename ...JT>
                bool operator=(const json_t<JT...> &json)
                {
                    bool rtn = false;
                    rtn |= json("sampler"s, sampler);
                    rtn |= json("target"s, target);
                    return rtn;
                }
                template<typename ...JT>
                friend inline void operator<<(json_t<JT...> &json, const channel_t &channel)
                {
                    json = json_object_t<JT...>{};
                    json["sampler"s] = channel.sampler;
                    json["target"s] = channel.target;
                }
            };
            struct sampler_t
            {
                size_t input;
                size_t output;
                string interpolation;
                template<typename ...JT>
                bool operator=(const json_t<JT...> &json)
                {
                    bool rtn = false;
                    rtn |= json("input"s, input);
                    rtn |= json("output"s, output);
                    rtn |= json("interpolation"s, interpolation);
                    return rtn;
                }
                template<typename ...JT>
                friend inline void operator<<(json_t<JT...> &json, const sampler_t &sampler)
                {
                    json = json_object_t<JT...>{};
                    json["inputs"s] = sampler.input;
                    json["output"s] = sampler.output;
                    json["interpolation"s] = sampler.interpolation;
                }
            };
            string name;
            vector<channel_t> channels;
            vector<sampler_t> samplers;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("name"s, name);
                rtn |= json("channels"s, channels);
                rtn |= json("samplers"s, samplers);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const animation_t &animation)
            {
                json = json_object_t<JT...>{};
                json["name"s] = animation.name;
                json["channels"s] = animation.channels;
                json["samplers"s] = animation.samplers;
            }
        };

        struct asset_t
        {
            string version;
            string generator;
            string copyright;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("version"s, version);
                rtn |= json("generator"s, generator);
                rtn |= json("copyright"s, copyright);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const asset_t &asset)
            {
                json = json_object_t<JT...>{};
                json["version"] = asset.version;
                json["generator"] = asset.generator;
                json["copyright"] = asset.copyright;
            }
        };

        struct buffer_t
        {
            size_t byteLength;
            string uri;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("byteLength"s, byteLength);
                rtn |= json("uri"s, uri);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const buffer_t &buffer)
            {
                json = json_object_t<JT...>{};
                json["byteLength"s] = buffer.byteLength;
                json["uri"s] = buffer.uri;
            }
        };

        struct bufferView_t
        {
            size_t buffer;
            size_t byteLength;
            size_t byteOffset;
            size_t byteStride;
            GLenum target;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("buffer"s, buffer);
                rtn |= json("byteLength"s, byteLength);
                rtn |= json("byteOffset"s, byteOffset);
                rtn |= json("byteStride"s, byteStride);
                rtn |= json("target"s, target);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const bufferView_t &bufferView)
            {
                json = json_object_t<JT...>{};
                json["buffer"s] = bufferView.buffer;
                json["byteLength"s] = bufferView.byteLength;
                json["byteOffset"s] = bufferView.byteOffset;
                json["byteStride"s] = bufferView.byteStride;
                json["target"s] = bufferView.target;
            }
        };

        struct camera_t
        {
            struct perspective_t
            {
                double aspectRatio;
                double yfov;
                double zfar;
                double znear;
                template<typename ...JT>
                bool operator=(const json_t<JT...> &json)
                {
                    bool rtn = false;
                    rtn |= json("aspectRatio"s, aspectRatio);
                    rtn |= json("yfov"s, yfov);
                    rtn |= json("zfar"s, zfar);
                    rtn |= json("znear"s, znear);
                    return rtn;
                }
                template<typename ...JT>
                friend inline void operator<<(json_t<JT...> &json, const perspective_t &perspective)
                {
                    json = json_object_t<JT...>{};
                    json["aspectRatio"s] = perspective.aspectRatio;
                    json["yfov"s] = perspective.yfov;
                    json["zfar"s] = perspective.zfar;
                    json["znear"s] = perspective.znear;
                }
            };
            struct orthographic_t
            {
                double xmag;
                double ymag;
                double zfar;
                double znear;
                template<typename ...JT>
                bool operator=(const json_t<JT...> &json)
                {
                    bool rtn = false;
                    rtn |= json("xmag"s, xmag);
                    rtn |= json("ymag"s, ymag);
                    rtn |= json("zfar"s, zfar);
                    rtn |= json("znear"s, znear);
                    return rtn;
                }
                template<typename ...JT>
                friend inline void operator<<(json_t<JT...> &json, const orthographic_t &orthographic)
                {
                    json = json_object_t<JT...>{};
                    json["xmag"s] = orthographic.xmag;
                    json["ymag"s] = orthographic.ymag;
                    json["zfar"s] = orthographic.zfar;
                    json["znear"s] = orthographic.znear;
                }
            };
            string name;
            enum { ERROR, PERSPECTIVE, ORTHOGRAPHIC } type;
            union
            {
                perspective_t perspective;
                orthographic_t orthographic;
            };
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("name"s, name);
                string _type;
                rtn |= json("type"s, _type);
                if (_type == "orthographic"s)
                {
                    type = ORTHOGRAPHIC;
                    rtn |= json("orthographic"s, orthographic);
                }
                else if (_type == "perspective"s)
                {
                    type = PERSPECTIVE;
                    rtn |= json("perspective"s, perspective);
                }
                else
                {
                    type = ERROR;
                }
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const camera_t &camera)
            {
                json = json_object_t<JT...>{};
                json["name"s] = camera.name;
                if (camera.type == PERSPECTIVE)
                {
                    json["type"s] = "perspective"s;
                    json["perspective"s] = camera.perspective;
                }
                else if (camera.type == ORTHOGRAPHIC)
                {
                    json["type"s] = "orthographic"s;
                    json["orthographic"s] = camera.orthographic;
                }
            }
        };

        struct image_t
        {
            string mimeType;
            // use empty string ("") to mark that we're using bufferView
            string uri;
            size_t bufferView;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("mimeType"s, mimeType);
                if (json("uri"s, uri))
                    rtn |= true;
                else
                    uri = "";
                rtn |= json("bufferView"s, bufferView);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const image_t &image)
            {
                json = json_object_t<JT...>{};
                json["mimeType"s] = image.mimeType;
                if (image.uri != "")
                    json["uri"s] = image.uri;
                else
                    json["bufferView"s] = image.bufferView;
            }
        };

        struct material_t
        {
            string name;
            JSON details;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("name"s, name);
                if (const JSON *obj = json("details"s); obj)
                {
                    details = *obj;
                    rtn |= true;
                }
                return true;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const material_t &material)
            {
                json = json_object_t<JT...>{};
                json["name"s] = material.name;
                json["details"s] = material.details; // JSON -> json_t<JT...> conversion
            }
        };

        struct mesh_t
        {
            struct primitive_t
            {
                unordered_map<string, size_t> attributes;
                unordered_map<string, size_t> targets;
                size_t indices;
                size_t material;
                size_t mode;
                template<typename ...JT>
                bool operator=(const json_t<JT...> &json)
                {
                    bool rtn = false;
                    rtn |= json("attributes"s, attributes);
                    rtn |= json("targets"s, targets);
                    rtn |= json("indices"s, indices);
                    rtn |= json("material"s, material);
                    rtn |= json("mode"s, mode);
                    return rtn;
                }
                template<typename ...JT>
                friend inline void operator<<(json_t<JT...> &json, const primitive_t &primitive)
                {
                    json = json_object_t<JT...>{};
                    json["attributes"s] = primitive.attributes;
                    json["targets"s] = primitive.targets;
                    json["indices"s] = primitive.indices;
                    json["material"s] = primitive.material;
                    json["mode"s] = primitive.mode;
                }
            };
            string name;
            vector<primitive_t> primitives;
            vector<double> weights;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("name"s, name);
                rtn |= json("primitives"s, primitives);
                rtn |= json("weights"s, weights);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const mesh_t &mesh)
            {
                json = json_object_t<JT...>{};
                json["name"s] = mesh.name;
                json["primitives"s] = mesh.primitives;
                json["weights"s] = mesh.weights;
            }
        };

        struct node_t
        {
            string name;
            size_t mesh;
            size_t skin;
            // NOTE: Must be TRS *OR* matrix, not both!
            glm::vec3 translation;
            glm::vec4 rotation;
            glm::vec3 scale;
            glm::mat4 matrix;
            vector<size_t> children;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("name"s, name);
                rtn |= json("mesh"s, mesh);
                rtn |= json("skin"s, skin);
                // TODO - translation
                // TODO - rotation
                // TODO - scale
                // TODO - matrix
                rtn |= json("children"s, children);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const node_t &node)
            {
                json = json_object_t<JT...>{};
                json["name"s] = node.name;
                json["skin"s] = node.skin;
                // TODO - translation
                // TODO - rotation
                // TODO - scale
                // TODO - matrix
                json["children"s] = node.children;
            }
        };

        struct sampler_t
        {
            GLenum magFilter;
            GLenum minFilter;
            GLenum wrapS;
            GLenum wrapT;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("magFilter"s, magFilter);
                rtn |= json("minFilter"s, minFilter);
                rtn |= json("wrapS"s, wrapS);
                rtn |= json("wrapT"s, wrapT);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const sampler_t &sampler)
            {
                json = json_object_t<JT...>{};
                json["magFilter"s] = sampler.magFilter;
                json["minFilter"s] = sampler.minFilter;
                json["wrapS"s] = sampler.wrapS;
                json["wrapT"s] = sampler.wrapT;
            }
        };

        struct scene_t
        {
            string name;
            vector<size_t> nodes;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("name"s, name);
                rtn |= json("nodes"s, nodes);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const scene_t &scene)
            {
                json = json_object_t<JT...>{};
                json["name"s] = scene.name;
                json["nodes"s] = scene.nodes;
            }
        };

        struct skin_t
        {
            string name;
            size_t inverseBindMatrices;
            size_t skeleton;
            vector<size_t> joints;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("name"s, name);
                rtn |= json("inverseBindMatrices"s, inverseBindMatrices);
                rtn |= json("skeleton"s, skeleton);
                rtn |= json("joints"s, joints);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const skin_t &skin)
            {
                json = json_object_t<JT...>{};
                json["name"s] = skin.name;
                json["inverseBindMatrices"s] = skin.inverseBindMatrices;
                json["skeleton"s] = skin.skeleton;
                json["joints"s] = skin.joints;
            }
        };

        struct texture_t
        {
            size_t sampler;
            size_t source;
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                rtn |= json("sampler"s, sampler);
                rtn |= json("source"s, source);
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const texture_t &texture)
            {
                json = json_object_t<JT...>{};
                json["sampler"s] = texture.sampler;
                json["source"s] = texture.source;
            }
        };

        struct extensions_t
        {
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const extensions_t &extensions)
            {
                json = json_object_t<JT...>{};
            }
        };

        struct extras_t
        {
            template<typename ...JT>
            bool operator=(const json_t<JT...> &json)
            {
                bool rtn = false;
                return rtn;
            }
            template<typename ...JT>
            friend inline void operator<<(json_t<JT...> &json, const extras_t &extras)
            {
                json = json_object_t<JT...>{};
            }
        };

        vector<string> extensionsUsed;
        vector<string> extensionsRequired;
        vector<accessor_t> accessors;
        vector<animation_t> animations;
        asset_t asset;                      // required
        vector<buffer_t> buffers;
        vector<bufferView_t> bufferViews;
        vector<camera_t> cameras;
        vector<image_t> images;
        vector<material_t> materials;
        vector<mesh_t> meshes;
        vector<node_t> nodes;
        vector<sampler_t> samplers;
        size_t scene;                       // default scene (scene to show at load)
        vector<scene_t> scenes;
        vector<skin_t> skins;
        vector<texture_t> textures;
        extensions_t extensions;
        extras_t extras;

        gltf_t() {}
        ~gltf_t() {}

        template<typename ...JT>
        gltf_t(const json_t<JT...> &json)
        {
            *this = json;
        }

        template<typename ...JT>
        inline bool operator=(const json_t<JT...> &json)
        {
            bool rtn = false;
            rtn |= json("extensionsUsed"s, extensionsUsed);
            rtn |= json("extensionsRequired"s, extensionsRequired);
            rtn |= json("accessors"s, accessors);
            rtn |= json("animations"s, animations);
            rtn |= json("asset"s, asset);
            rtn |= json("buffers"s, buffers);
            rtn |= json("bufferViews"s, bufferViews);
            rtn |= json("cameras"s, cameras);
            rtn |= json("images"s, images);
            rtn |= json("materials"s, materials);
            rtn |= json("meshes"s, meshes);
            rtn |= json("nodes"s, nodes);
            rtn |= json("samplers"s, samplers);
            rtn |= json("scene"s, scene);
            rtn |= json("scenes"s, scenes);
            rtn |= json("skins"s, skins);
            rtn |= json("textures"s, textures);
            rtn |= json("extensions"s, extensions);
            rtn |= json("extras"s, extras);
            return rtn;
        }

        template<typename ...JT>
        friend inline void operator<<(json_t<JT...> &json, const gltf_t &gltf)
        {
            json = json_object_t<JT...>{};
            json["extensionsUsed"s] = gltf.extensionsUsed;
            json["extensionsRequired"s] = gltf.extensionsRequired;
            json["accessors"s] = gltf.accessors;
            json["animations"s] = gltf.animations;
            json["asset"s] = gltf.asset;
            json["buffers"s] = gltf.buffers;
            json["bufferViews"s] = gltf.bufferViews;
            json["cameras"s] = gltf.cameras;
            json["images"s] = gltf.images;
            json["materails"s] = gltf.materials;
            json["meshes"s] = gltf.meshes;
            json["nodes"s] = gltf.nodes;
            json["samplers"s] = gltf.samplers;
            json["scene"s] = gltf.scene;
            json["scenes"s] = gltf.scenes;
            json["skins"s] = gltf.skins;
            json["textures"s] = gltf.textures;
            json["extensions"s] = gltf.extensions;
            json["extras"s] = gltf.extras;
        }
    };
}

#endif // LAK_GLTF_H