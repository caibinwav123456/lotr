//
//effect file for lord of the rings
//

//
//globals
//

//VS
float4 screensize;

//PS

//textures
Texture Tex;
Texture Mask<string name="refine_output.bmp";>;

//
//samplers
//

sampler2D s0=sampler_state
{
	Texture=(Tex);
	MinFilter=POINT;
	MagFilter=POINT;
	MipFilter=POINT;
	AddressU=CLAMP;
	AddressV=CLAMP;
};

sampler2D s1=sampler_state
{
	Texture=(Tex);
	MinFilter=LINEAR;
	MagFilter=LINEAR;
	MipFilter=LINEAR;
	AddressU=CLAMP;
	AddressV=CLAMP;
};

sampler2D s2=sampler_state
{
	Texture=(Mask);
	MinFilter=LINEAR;
	MagFilter=LINEAR;
	MipFilter=LINEAR;
	AddressU=CLAMP;
	AddressV=CLAMP;
};

//
//structures
//

struct VS_INPUT
{
	float4 inpos : POSITION;
	float3 normal: NORMAL0;
	float2 tex   : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 outpos : POSITION;
	float2 tex    : TEXCOORD0;
};

struct PS_INPUT
{
	float4 inpos : POSITION;
	float2 tex   : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

//
//vs main function
//

VS_OUTPUT vs_main(VS_INPUT input)
{
	VS_OUTPUT output=(VS_OUTPUT)0;
	float2 pos=input.inpos.xy;
	float2 tpos=pos;
	output.outpos=float4(tpos.xy,0,1);
	output.tex=input.tex;
	return output;
}

//
//ps main function
//

PS_OUTPUT ps_main(PS_INPUT input)
{
	PS_OUTPUT output=(PS_OUTPUT)0;
	float2 tex=input.tex+float2(0.5,0.5)/screensize.xy;
	float4 pix=tex2D(s0,tex);
	float c=pix.r;
	float l=tex2D(s0,tex+float2(-1.0/screensize.x,0)).r;
	float r=tex2D(s0,tex+float2( 1.0/screensize.x,0)).r;
	float t=tex2D(s0,tex+float2(0,-1.0/screensize.y)).r;
	float b=tex2D(s0,tex+float2(0, 1.0/screensize.y)).r;
	float4 pmask=tex2D(s2,tex);
	float acc=0.01;
	float color=((l+r+t+b)/4+acc)*pmask.r;
	output.color=float4(color,color,color,1);

	return output;
}

PS_OUTPUT ps_main_final(PS_INPUT input)
{
	PS_OUTPUT output=(PS_OUTPUT)0;
	float2 tex=input.tex+float2(0.5,0.5)/screensize.xy;
	float l=tex2D(s1,tex+float2(-1.0/screensize.x,0)).r;
	float r=tex2D(s1,tex+float2( 1.0/screensize.x,0)).r;
	float t=tex2D(s1,tex+float2(0,-1.0/screensize.y)).r;
	float b=tex2D(s1,tex+float2(0, 1.0/screensize.y)).r;
	float3 n=float3(b-t,r-l,0);
	float pmask=tex2D(s2,tex).r;
	output.color=float4(n*3,pmask);
	
	return output;
}

PS_OUTPUT ps_main_out(PS_INPUT input)
{
	PS_OUTPUT output=(PS_OUTPUT)0;
	float asp=(screensize.x/screensize.y)/(screensize.z/screensize.w)*4;
	float2 text=input.tex;
	text.y=0.5+(text.y-0.5)*asp;
	if(text.y>=0&&text.y<=1)
	{
		float2 tex;
		if(text.x<=0.5)
		{
			tex.x=text.x*2;
			tex.y=text.y/2;
		}
		else
		{
			tex.x=text.x*2-1;
			tex.y=(text.y+1)/2;
		}
		output.color=tex2D(s1,tex);
	}
	return output;
}

Technique T0
{
	Pass P0
	{
		VertexShader=compile vs_2_0 vs_main();
		PixelShader=compile ps_2_0 ps_main();
	}
	Pass P1
	{
		VertexShader=compile vs_2_0 vs_main();
		PixelShader=compile ps_2_0 ps_main_final();
	}
	Pass P2
	{
		VertexShader=compile vs_2_0 vs_main();
		PixelShader=compile ps_2_0 ps_main_out();
	}
}