#version 330 core
out vec4 FragColor;

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirectionalLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    vec3 position;

    float constant;
    float linear; 
    float quadratic; 

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight{
    vec3 position;
    vec3 direction;

    float innerCutOff;
    float OuterCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


#define NR_POINT_LIGHTS 4


uniform DirectionalLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform Material material;
uniform vec3 viewPos;

vec3 CalculateDirectionalLight(DirectionalLight dirLight, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


void main()
{   
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalculateDirectionalLight(dirLight, normal, viewDir);

    for(int i = 0; i < NR_POINT_LIGHTS; i++)
    {
        result += CalculatePointLight(pointLights[i], normal, FragPos, viewDir);
    }

    result += CalculateSpotLight(spotLight, normal, FragPos, viewDir);


    FragColor = vec4(result, 1.0);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 diffuseTexture = vec3(texture(material.diffuse, TexCoords));

    vec3 ambient = light.ambient * diffuseTexture;
    vec3 diffuse = light.diffuse * diff  * diffuseTexture;
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant * light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuseTexture = vec3(texture(material.diffuse, TexCoords));

    vec3 ambient = light.ambient * attenuation * diffuseTexture;
    vec3 diffuse = light.diffuse * diff * attenuation * diffuseTexture;
    vec3 specular = light.specular * diff * attenuation * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);

}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float theta = dot(viewDir, lightDir);
    float epsilon = light.innerCutOff - light.OuterCutOff;
    float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

    
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant * light.linear * distance + light.quadratic * (distance * distance));

    vec3 diffuseTexture = vec3(texture(material.diffuse, TexCoords));

    vec3 ambient = light.ambient * intensity * attenuation * diffuseTexture;
    vec3 diffuse = light.diffuse * diff * intensity * attenuation * diffuseTexture;
    vec3 specular = light.specular * diff * intensity * attenuation * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
   
}