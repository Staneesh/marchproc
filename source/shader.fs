

#version 330 core
in vec3 FragPos;
out vec4 FragColor;

float sdf_sphere(vec3 p, vec3 c, float r)
{
	return length(c - p) - r;
}
		

void main()
{
	vec3 p = FragPos.xyz;

	float d = sdf_sphere(p, vec3(0.0f, 0.0f, -1.0f), 1.0f);

    FragColor = vec4(d, 0.0f, 0.0f, 1.0f);
}

