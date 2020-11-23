#include "TwinklesSystem.h"

TwinklesSystem::TwinklesSystem(const char* filepath)
{
	std::cout << "Loading Twinkles particle system " << filepath << "\n";

	IOArchive Ar(filepath, IODirection::Import);
	if (!Serialize(Ar))
	{
		std::cout << "Particle load failed!\n";
	}
}

void TwinklesSystem::Export(const char* filepath)
{
	std::cout << "Exporting Twinkles particle system " << filepath << "\n";
	IOArchive Ar(filepath, IODirection::Export);
	if (!Serialize(Ar))
	{
		std::cout << "Particle save failed!\n";
	}
}

bool TwinklesSystem::Serialize(IOArchive& Ar)
{
	if (!Ar.ChunkHeader("XFPT")) return false;
	Ar << Version;
	if (Version != 104 && Version != 105)
	{
		std::cout << "Unsupported particle version " << Version << "\n";
		return false;
	}

	uint32_t NumFx = Emitters.size();
	Ar << NumFx;
	if(Ar.IsLoading()) Emitters.resize(NumFx);
	for (int Fx = 0; Fx < NumFx; Fx++)
	{
		std::cout << "Emitter " << Fx << "\n";
		Emitter& Emit = Emitters[Fx];
		Emit.Serialize(Ar, Version);
	}

	return true;
}

bool Emitter::Serialize(IOArchive& Ar, uint32_t Version)
{
	if (!Ar.ChunkHeader("XFTS")) return false;
	Ar << TextureKUID;
	std::cout << "material KUID <kuid:" << TextureKUID.UserID << ":" << TextureKUID.ContentID << ">\n";
	//x, z, y?
	Ar << Position;
	std::cout << "Position: " << Position.DebugString() << "\n";

	Ar << Rotation.x;
	Ar << Rotation.y;
	Ar << Rotation.z;
	Ar << Rotation.w;

	std::cout << "Emitter Size:\n";
	EmitterSize.Serialize(Ar);

	std::cout << "Emission Rate:\n";
	EmissionRate.Serialize(Ar);

	std::cout << "Velocity Cone:\n";
	VelocityCone.Serialize(Ar);

	std::cout << "Z Speed Variance:\n";
	ZSpeedVariance.Serialize(Ar);

	Ar << Type;
	//unknown float value
	float unknown = 1.0f;
	Ar << unknown;
	std::cout << "Unknown float: " << unknown << "\n";

	std::cout << "Lifetime:\n";
	Lifetime.Serialize(Ar);

	std::cout << "Lifetime Variance:\n";
	LifetimeVariance.Serialize(Ar);

	std::cout << "Size Range:\n";
	SizeRange.Serialize(Ar);

	std::cout << "Size Variance:\n";
	SizeVariance.Serialize(Ar);

	std::cout << "Particle Size:\n";
	Size.Serialize(Ar);

	std::cout << "Particle Color:\n";
	Color.Serialize(Ar);

	std::cout << "Max Rotation:\n";
	MaxRotation.Serialize(Ar); //radians

	std::cout << "Gravity:\n";
	Gravity.Serialize(Ar);

	Ar << VelocityMinSpeed;
	Ar << VelocityMaxSpeed;

	std::cout << "Wind Factor:\n";
	WindFactor.Serialize(Ar);

	if(Version > 104) Ar << VelocityDampening;

	if (!Ar.ChunkHeader("XFDE")) return false;
	return true;
}

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

Vector2 lerp(const Vector2& a, const Vector2& b, float f)
{
	return Vector2(lerp(a.x, b.x, f), lerp(a.y, b.y, f));
}

Vector3 lerp(const Vector3& a, const Vector3& b, float f)
{
	return Vector3(lerp(a.x, b.x, f), lerp(a.y, b.y, f), lerp(a.z, b.z, f));
}

Color lerp(const Color& a, const Color& b, float f)
{
	return Color(lerp((float)a.r, (float)b.r, f),
				 lerp((float)a.g, (float)b.g, f),
				 lerp((float)a.b, (float)b.b, f),
				 lerp((float)a.a, (float)b.a, f));
}

//float Emitter::GetFloatKey(float time, KeyframeTrack<float>& Track)
//{
	//auto Start = Track.Frames.lower_bound(time);
	//auto End = Track.Frames.upper_bound(time);

	//float StartKey = Start->first;
	//float EndKey = End->first;
	//auto& StartFrame = Start->second;
	//auto& EndFrame = End->second;

	//float Along = (time - StartKey) / (EndKey - StartKey);
	//return lerp(StartFrame, EndFrame, Along);
//}
//
//Vector3 Emitter::GetVectorKey(float time, KeyframeTrack<Vector3>& Track)
//{
//	return Vector3();
//}

KeyframeTrackBase::~KeyframeTrackBase()
{

}

template<class T>
T KeyframeTrack<T>::GetKey(float time)
{
	//auto Start = Frames.lower_bound(time);
	//auto End = Frames.upper_bound(time);
	auto Start = GetLastFrame(time);
	auto End = GetNextFrame(time);

	float StartKey = Start.first;
	float EndKey = End.first;
	auto& StartFrame = Start.second;
	auto& EndFrame = End.second;

	//prevent divide by zero
	if (EndKey == StartKey)
		EndKey += 0.01f;

	//std::cout << "time " << time << ":\n";
	//std::cout << "start " << StartKey << ": " << StartFrame << "\n";
	//std::cout << "end " << EndKey << ": " << EndFrame << "\n";

	float Along = (time - StartKey) / (EndKey - StartKey);
	return lerp(StartFrame, EndFrame, Along);
}

//explicit instantiation
template class KeyframeTrack<float>;
template class KeyframeTrack<Vector2>;
template class KeyframeTrack<Vector3>;
template class KeyframeTrack<Color>;