#pragma once
#include "Math4.h"
#include "Material.h"
#include <basetsd.h>
struct light {
	double intensity;
	virtual UINT32 colorCalc(const Material& material, const Vector4& normal, const Vector4& polyPos, const Vector4& camPos) const = 0;
	static  UINT32 superPosLights(const light& l1, const light& l2, const light& l3, const Material& material, const Vector4& normal, const Vector4& polyPos, const Vector4& camPos)
	{
		UINT32 c1 = l1.colorCalc(material, normal, polyPos, camPos);
		UINT32 c2 = l2.colorCalc(material, normal, polyPos, camPos);
		UINT32 c3 = l3.colorCalc(material, normal, polyPos, camPos);
		UINT8* b1 = (UINT8*)&c1;
		UINT8* b2 = (UINT8*)&c2;
		UINT8* b3 = (UINT8*)&c3;
		UINT32 result;
		UINT8* byteResult = (UINT8*)&result;
		for (int i = 0; i <= 2; i++)
		{
			byteResult[i] = (UINT8)std::min(255.0, (double)(b1[i] + b2[i] + b3[i]));
		}
		return result;
	}
};

struct directionalLight :light {
	Vector4 direction;
	virtual UINT32 colorCalc(const Material& material, const Vector4& normal, const Vector4& polyPos, const Vector4& camPos) const
	{
		Vector4 directionNormalized = direction.normalize();
		Vector4 cameraNormal = camPos - polyPos;
		cameraNormal.w = 1;
		cameraNormal = cameraNormal.normalize();
		double angleNormalLight = normal.dotEuclid(directionNormalized);
		angleNormalLight = std::max(0.0, angleNormalLight);
		Vector4 reflectedLight = -directionNormalized + 2 * (angleNormalLight*normal);
		reflectedLight.w = 1;
		double angleReflectedCamera = std::max(0.0, reflectedLight.dotEuclid(cameraNormal));
		UINT32 color;
		UINT8* byteColor = (UINT8*)&color;
		UINT8* byteDiffuse = (UINT8*)&material.kDiffuse;
		UINT8* byteSpecular = (UINT8*)&material.kSpecular;
		for (int i = 0; i <= 2; i++)
		{
			byteColor[i] = (UINT8)std::min(255.0, intensity * (angleNormalLight * byteDiffuse[i] + std::pow(angleReflectedCamera, material.normSpecular)*byteSpecular[i]));
		}
		return color;

	}
};

struct pointLight :light {
	Vector4 position;
	virtual UINT32 colorCalc(const Material& material, const Vector4& normal, const Vector4& polyPos, const Vector4& camPos) const
	{
		Vector4 relativeDirection = position - polyPos;
		Vector4 cameraNormal = camPos - polyPos;
		relativeDirection.w = 1;
		cameraNormal.w = 1;
		relativeDirection = relativeDirection.normalize();
		cameraNormal = cameraNormal.normalize();
		double angleNormalLight = normal.dotEuclid(relativeDirection);
		angleNormalLight = std::max(0.0, angleNormalLight);
		Vector4 reflectedLight = -relativeDirection + 2 * (angleNormalLight*normal);
		reflectedLight.w = 1;
		double angleReflectedCamera = std::max(0.0,reflectedLight.dotEuclid(cameraNormal));
		UINT32 color;
		UINT8* byteColor=(UINT8*)&color;
		UINT8* byteDiffuse = (UINT8*)&material.kDiffuse;
		UINT8* byteSpecular = (UINT8*)&material.kSpecular;
		for (int i = 0; i <= 2; i++)
		{
			byteColor[i] = (UINT8)std::min(255.0,intensity * (angleNormalLight * byteDiffuse[i] + std::pow(angleReflectedCamera, material.normSpecular)*byteSpecular[i]));
		}
		return color;

	}
	
};
struct ambientLight :light {

	virtual UINT32 colorCalc(const Material& material, const Vector4& normal, const Vector4& polyPos, const Vector4& camPos) const
	{
		UINT32 polyColor=0xff000000;
		UINT8* byteColor = (UINT8*)&polyColor;
		UINT8* kAmbientByte = (UINT8*)&material.kAmbient;
		for (int i = 0; i <= 2; i++)
		{
			byteColor[i] = kAmbientByte[i] * intensity;
		}
		return polyColor;

	}



};