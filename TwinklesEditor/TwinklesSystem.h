#pragma once
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
};

struct Vector3
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	friend IOArchive& operator<<(IOArchive& Ar, Vector3& vec)
	{
		Ar << vec.x;
		Ar << vec.y;
		Ar << vec.z;
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
};

struct Quaternion
{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 1.0f;
};

struct Color
{
	uint8_t r = 0x00;
	uint8_t g = 0x00;
	uint8_t b = 0x00;
	uint8_t a = 0xFF;
	friend IOArchive& operator<<(IOArchive& Ar, Color& col)
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

	friend std::ostream& operator<<(std::ostream& out, const Color& col)
	{
		out << col.DebugString();
		return out;
	}
};

template <class T>
struct Keyframe
{
public:
	float Key;
	T Value;
	friend IOArchive& operator<<(IOArchive& Ar, Keyframe& frame)
	{
		Ar << frame.Key;
		Ar << frame.Value;
		return Ar;
	}
};

template <class T>
class KeyframeTrack
{
public:
	std::vector<Keyframe<T>> Frames;
	bool Serialize(IOArchive& Ar)
	{
		uint32_t FrameNum = Frames.size();
		Ar << FrameNum;
		if (Ar.IsLoading()) Frames.resize(FrameNum);
		std::cout << FrameNum << " frames:\n";
		for (int i = 0; i < FrameNum; i++)
		{
			Keyframe<T>& Frame = Frames[i];
			Ar << Frame;
			std::cout << Frame.Key << ": " << Frame.Value << "\n";
		}
		return true;
	}
};

class Emitter
{
	KUID TextureKUID;
	Vector3 Position;
	Quaternion Rotation;
	//Frames
	KeyframeTrack<Vector3> EmitterSize;
	KeyframeTrack<float> EmissionRate;
	KeyframeTrack<Vector3> VelocityCone;
	KeyframeTrack<float> ZSpeedVariance;
	ParticleType Type;
	KeyframeTrack<float> Lifetime;
	KeyframeTrack<float> LifetimeVariance;
	KeyframeTrack<Vector2> SizeRange;
	KeyframeTrack<float> SizeVariance;
	KeyframeTrack<float> Size;
	KeyframeTrack<Color> Color;
	KeyframeTrack<float> MaxRotation; //radians
	KeyframeTrack<float> Gravity;
	float VelocityMinSpeed = -254.0;
	float VelocityMaxSpeed = 254.0;
	KeyframeTrack<float> WindFactor;
	Vector3 VelocityDampening;

public:
	bool Serialize(IOArchive& Ar);
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
};

