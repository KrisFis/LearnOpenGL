#version 330 core

out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D screenTexture;

vec4 WeightedGray(vec4 TextureCoord)
{
	float average = 0.2126 * TextureCoord.r + 0.7152 * TextureCoord.g + 0.0722 * TextureCoord.b;
	return vec4(average, average, average, 1.0);
}

vec4 UnweightedGray(vec4 TextureCoord)
{
	float average = (TextureCoord.r + TextureCoord.g + TextureCoord.b) / 3.0;
 	return vec4(average, average, average, 1.0);
}

void main()
{
	texCoord = texture(screenTexture, TexCoord);
	
	FragColor = WeightedGray(texCoord);
	//FragColor = UnweightedGray(texCoord);
}