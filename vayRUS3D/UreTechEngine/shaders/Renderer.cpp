#include"Renderer.hpp"
#include<glm/mat3x3.hpp>
#include<../EngineCore.h>

Renderer::Renderer()
{
	programID = glCreateProgram();
}

Renderer::~Renderer()
{
	glDeleteProgram(programID);
}

void Renderer::link()
{
	glLinkProgram(programID);

	int isLinkd;
	char log[512];
	glGetProgramiv(programID, GL_LINK_STATUS, &isLinkd);
	if (!isLinkd) {
		glGetProgramInfoLog(programID, 512, 0, log);
		UreTechEngine::EngineConsole::log("Shader Linking Error!\n" + string(log), UreTechEngine::EngineConsole::ERROR_FATAL);
	}
}

void Renderer::use()
{
	glUseProgram(programID);
}

void Renderer::addUniform(const std::string& varName)
{
	m_UniformVars[varName] = glGetUniformLocation(programID, varName.c_str());
	if ((int)m_UniformVars[varName] == -1) {
		UreTechEngine::EngineConsole::log(string::stdStrToUStr(varName) + " " + string::stdStrToUStr(std::to_string(m_UniformVars[varName])) + " is missing!", UreTechEngine::EngineConsole::WARN_CAN_CAUSE_ERROR);
	}
	else {
		UreTechEngine::EngineConsole::log(string::stdStrToUStr(varName) + " " + string::stdStrToUStr(std::to_string(m_UniformVars[varName])), UreTechEngine::EngineConsole::INFO_NORMAL);
	}
}

void Renderer::setFloat(const std::string& varName, float val)
{
	glUniform1f(m_UniformVars[varName], val);
}

void Renderer::setBool(const std::string& varName, bool val)
{
	glUniform1i(m_UniformVars[varName], val);
}

void Renderer::setInt(const std::string& varName, int val)
{
	glUniform1i(m_UniformVars[varName], val);
}

void Renderer::setVec3(const std::string& varName, glm::vec3 val)
{
	glUniform3f(m_UniformVars[varName], val.x,val.y,val.z);
}

void Renderer::setVec4(const std::string& varName, glm::vec4& val)
{
	glUniform4f(m_UniformVars[varName], val.x,val.y,val.z,val.w);
}

void Renderer::setMat3(const std::string& varName, glm::mat3* val)
{
	glUniformMatrix3fv(m_UniformVars[varName],1,false,(GLfloat*)val);
}

void Renderer::setMat4(const std::string& varName, glm::mat4* val)
{
	glUniformMatrix4fv(m_UniformVars[varName],1,false, (GLfloat*)val);
}

void Renderer::setTexture(const std::string& varName, int _textureLvl)
{
	//UreTechEngine::EngineERROR::consoleError(varName + std::to_string(m_UniformVars[varName]),UreTechEngine::EngineERROR::INFO_NORMAL);
	glUniform1i(m_UniformVars[varName], _textureLvl);
}

void Renderer::InitVulkan()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "vayRUS 3D Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		EngineConsole::log("Can not create Vulkan instance!", UreTechEngine::EngineConsole::ERROR_FATAL);
    }

    // 2. Fiziksel cihazý seç
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        EngineConsole::log("Can not find available device for Vulkan!", UreTechEngine::EngineConsole::ERROR_FATAL);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    physicalDevice = devices[0];

    // 3. Mantýksal cihaz (Logical Device) oluþtur
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = 0;  // Varsayýlan olarak ilk sýrayý kullan
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        EngineConsole::log("Error while creating logical device!", UreTechEngine::EngineConsole::ERROR_FATAL);
    }

    vkGetDeviceQueue(device, 0, 0, &graphicsQueue);

    // 4. Render Pass oluþtur
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        EngineConsole::log("Render pass ERROR!", UreTechEngine::EngineConsole::ERROR_FATAL);
    }
}

void Renderer::InitImGuiVulkan()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForVulkan(this->window, true);

    VkDescriptorPoolSize pool_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000;
    pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    if (vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool) != VK_SUCCESS) {
        EngineConsole::log("Descriptor Pool ERROR!", UreTechEngine::EngineConsole::ERROR_FATAL);
    }

    // 4. ImGui Vulkan backend baþlat
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance;
    init_info.PhysicalDevice = physicalDevice;
    init_info.Device = device;
    init_info.Queue = graphicsQueue;
    init_info.DescriptorPool = descriptorPool;
    init_info.MinImageCount = 2;
    init_info.ImageCount = 2;
    init_info.RenderPass = renderPass;

    ImGui_ImplVulkan_Init(&init_info);
}

 void Renderer::attachShader(const char* fileName, unsigned int shaderType)
{
	 unsigned int shaderID = glCreateShader(shaderType);
	 UreTechEngine::string data = getShaderFromFile(fileName);
	 const char* sourceCode = data.c_str();

	 glShaderSource(shaderID, 1, &sourceCode, 0);

	 glCompileShader(shaderID);

	 int isCompl;
	 char log[512];
	 glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompl);
	 if (!isCompl) {
		 glGetShaderInfoLog(shaderID, 512, 0, log);
		 UreTechEngine::EngineConsole::log("ShaderType:" + string::stdStrToUStr(std::to_string(shaderType)) + "\n" + log, UreTechEngine::EngineConsole::ERROR_FATAL);
	 }

	 glAttachShader(programID, shaderID);

	 glDeleteShader(shaderID);
}

 UreTechEngine::string Renderer::getShaderFromFile(const char* fileName)
 {
	 UreTechEngine::string data;
	 if (UPK_ENABLE_PACKAGE_SYSTEM) {
		 Buffer shaderBuf = UreTechEngine::UreTechEngineClass::getEngine()->package->get(fileName);
		 data = string((char*)shaderBuf.pointer, shaderBuf.size);
	 }
	 else {
		 std::ifstream file(fileName);
		 
		 if (file.is_open()) {
			 char readingChar;
			 while ((readingChar = file.get()) != EOF) {
				 data.push_back(readingChar);
			 }
			 file.close();
		 }
	 }
	 return data;
 }