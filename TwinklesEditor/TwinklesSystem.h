#pragma once
#include <typeinfo>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "IOArchive.h"

enum class ParticleType : uint32_t
{
	FaceCamera = 0x1,
	FaceMotion = 0x2,
	FaceDown = 0x4,
	FaceHorizontal = 0x10
};
struct KUID
{
public:
	int32_t UserID = -1;
	uint32_t ContentID = 6329;
	uint32_t Revision = 0;
	
	friend IOArchive& operator<<(IOArchive& Ar, KUID& kuid)
	{
		Ar << kuid.UserID;
		Ar << kuid.ContentID;
		Ar << kuid.Revision;
		return Ar;
	}
};

struct Vector2
{
	float x = 0.0f;
	float y = 0.0f;
	friend IOArchive& operator<<(IOArchive& Ar, Vector2& vec)
	{
		Ar << vec.x;
		Ar << vec.y;
		return Ar;
	}

	inline std::string DebugString() const
	{
		return "X: " + std::to_string(x) + " Y: " + std::to_string(y);
	}

	friend std::ostream& operator<<(std::ostream& out, const Vector2& vec)
	{
		out << vec.DebugString();
		return out;
	}

	Vector2(float InX, float InY) : x(InX), y(InY) { }
	Vector2() { }

	glm::vec2 ToGLM()
	{
		return glm::vec2(x, y);
	}
};

struct Vector3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	friend IOArchive& operator<<(IOArchive& Ar, Vector3& vec)
	{
		Ar << vec.x;
		Ar << vec.z;
		Ar << vec.y;
		return Ar;
	}
	inline std::string DebugString() const
	{
		return "X: " + std::to_string(x) + " Y: " + std::to_string(y) + " Z: " + std::to_string(z);
	}
	
	friend std::ostream& operator<<(std::ostream& out, const Vector3& vec)
	{
		out << vec.DebugString();
		return out;
	}

	Vector3(float InX, float InY, float InZ) : x(InX), y(InY), z(InZ) { }
	Vector3() { }

	glm::vec3 ToGLM()
	{
		return glm::vec3(x, y, z);
	}
};

struct Quaternion
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;

	glm::quat ToGLM()
	{
		//quaternion constructor is wxyz
		return glm::quat(w, x, y, z);
	}
};

struct TColor
{
	uint8_t r = 0x00;
	uint8_t g = 0x00;
	uint8_t b = 0x00;
	uint8_t a = 0xFF;
	friend IOArchive& operator<<(IOArchive& Ar, TColor& col)
	{
		//BGRA
		Ar << col.b;
		Ar << col.g;
		Ar << col.r;
		Ar << col.a;
		return Ar;
	}
	inline std::string DebugString() const
	{
		return "R: " + std::to_string(r) + " G: " + std::to_string(g) + " B: " + std::to_string(b) + " A: " + std::to_string(a);
	}

	friend std::ostream& operator<<(std::ostream& out, const TColor& col)
	{
		out << col.DebugString();
		return out;
	}

	TColor(uint8_t InR, uint8_t InG, uint8_t InB, uint8_t InA) : r(InR), g(InG), b(InB), a(InA) { }
	TColor() { }

	glm::vec4 ToGLM()
	{
		return glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, (float)a / 255.0f);
	}

	uint32_t ToHex()
	{
		//ABGR
		uint32_t Color = 0;
		Color |= r;
		Color |= g << 8;
		Color |= b << 16;
		Color |= a << 24;
		return Color;
	}

	void FromHex(uint32_t color)
	{
		uint8_t* colorPtr = (uint8_t*)&color;
		r = colorPtr[0];
		g = colorPtr[1];
		b = colorPtr[2];
		a = colorPtr[3];
	}

	bool operator==(const TColor& other) const
	{
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}
	bool operator<(const TColor& other) const
	{
		return r < other.r && g < other.g && b < other.b && a < other.a;
	}
};

float lerp(float a, float b, float f);
Vector2 lerp(const Vector2& a, const Vector2& b, float f);
Vector3 lerp(const Vector3& a, const Vector3& b, float f);
TColor lerp(const TColor& a, const TColor& b, float f);

//bool operator==(const std::pair<float, TColor>& A, const std::pair<float, TColor>& B)
//{
//	return A.first == B.first && A.second == B.second;
//}

//template <class T>
//struct Keyframe
//{
//public:
//	//float Key;
//	T Value;
//	friend IOArchive& operator<<(IOArchive& Ar, Keyframe& frame)
//	{
//		//Ar << frame.Key;
//		Ar << frame.Value;
//		return Ar;
//	}
//};

class KeyframeTrackBase
{
public:
	const type_info& type;
	const std::string name;
	KeyframeTrackBase(const type_info& InType, const std::string& InName) : type(InType), name(InName) { }
	virtual ~KeyframeTrackBase() = 0;
};

template <class T>
class KeyframeTrack : public KeyframeTrackBase
{
public:
	KeyframeTrack(const std::string& name) : KeyframeTrackBase(typeid(T), name) { }
	virtual ~KeyframeTrack() { }
	//std::vector<Keyframe<T>> Frames;
	//std::map<float, T> Frames;
	std::vector<std::pair<float, T>> Frames;
	//std::vector<float, uint32_t> Ranges;

	bool Serialize(IOArchive& Ar)
	{
		uint32_t FrameNum = Frames.size();
		Ar << FrameNum;
		//if (Ar.IsLoading()) Frames.resize(FrameNum);
		std::cout << FrameNum << " frames:\n";
		if (Ar.IsLoading())
		{
			Frames.clear();
			for (int i = 0; i < FrameNum; i++)
			{
				float key = 0.0f;
				T value;
				Ar << key;
				Ar << value;
				//Frames[key] = value;
				Frames.emplace_back(key, value);
				std::cout << key << ": " << value << "\n";
			}
		}
		else
		{
			for (auto& Frame : Frames)
			{
				float key = Frame.first;
				T value = Frame.second;
				Ar << key;
				Ar << value;
			}
		}
		return true;
	}


	std::pair<float, T> GetLastFrame(float time)
	{
		std::pair<float, T> closest = *Frames.begin();
		for (auto& Frame : Frames)
		{
			float distance = Frame.first - time;
			if (distance > 0.0f)
			{
				return closest;
			}
			closest = Frame;
		}
	}
	std::pair<float, T> GetNextFrame(float time)
	{
		for (auto& Frame : Frames)
		{
			float distance = Frame.first - time;
			if (distance > 0.0f)
			{
				return Frame;
			}
		}
		return *Frames.rbegin();
	}

	T GetKey(float time);
};

class RenderEmitter;

class Emitter
{
public:
	KUID TextureKUID;
	Vector3 Position;
	Quaternion Rotation;
	//Frames
	KeyframeTrack<Vector3> EmitterSize = KeyframeTrack<Vector3>("Emitter Size");
	KeyframeTrack<float> EmissionRate = KeyframeTrack<float>("Emission Rate");
	KeyframeTrack<Vector3> VelocityCone = KeyframeTrack<Vector3>("Velocity Cone");
	KeyframeTrack<float> ZSpeedVariance = KeyframeTrack<float>("Z Speed Variance");
	ParticleType Type;
	KeyframeTrack<float> Lifetime = KeyframeTrack<float>("Lifetime");
	KeyframeTrack<float> LifetimeVariance = KeyframeTrack<float>("Lifetime Variance");
	KeyframeTrack<Vector2> SizeRange = KeyframeTrack<Vector2>("Size Range");
	KeyframeTrack<float> SizeVariance = KeyframeTrack<float>("Size Variance");
	KeyframeTrack<float> Size = KeyframeTrack<float>("Size");
	KeyframeTrack<TColor> Color = KeyframeTrack<TColor>("Color");
	KeyframeTrack<float> MaxRotation = KeyframeTrack<float>("Max Rotation"); //radians
	KeyframeTrack<float> Gravity = KeyframeTrack<float>("Gravity");
	float VelocityMinSpeed = -254.0;
	float VelocityMaxSpeed = 254.0;
	KeyframeTrack<float> WindFactor = KeyframeTrack<float>("Wind Factor");
	Vector3 VelocityDampening;

	bool Serialize(IOArchive& Ar, uint32_t Version);
	
	RenderEmitter* renderEmitter;
	//float GetFloatKey(float time, KeyframeTrack<float>& Track);
	//Vector3 GetVectorKey(float time, KeyframeTrack<Vector3>& Track);
};

class TwinklesSystem
{
public:
	std::vector<Emitter> Emitters;
	uint32_t Version = 105;
private:
	bool Serialize(IOArchive& Ar);
public:
	TwinklesSystem(const char* filepath);
	void Export(const char* filepath);
	TwinklesSystem() {}
};
