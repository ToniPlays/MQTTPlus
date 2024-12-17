#include "FieldValueExpander.h"
#include "API/QueryTypes/TopicQueryType.h"

namespace MQTTPlus::API
{
    Promise<MQTTFieldValue> FieldValueExpander::Expand(const std::vector<MQTTFieldValue>& fields, const std::vector<std::string>& expandOpts)
    {
        std::vector<Ref<Job>> jobs;
        jobs.reserve(fields.size());
        
        for(auto& field : fields)
        {
            std::string fieldId = field.PublicID.Value();
            jobs.push_back(Job::Create(fieldId, FieldValueExpander::ExpandElement, field, expandOpts));
        }
            
        JobGraphInfo info = {
            .Name = "FieldValueExpander::Expand",
            .Stages = { { "Expand", 1.0, jobs } },
        };

        return ServiceManager::GetJobSystem()->Submit<MQTTFieldValue>(Ref<JobGraph>::Create(info));
    }

    Coroutine FieldValueExpander::ExpandElement(JobInfo info, MQTTFieldValue field, const std::vector<std::string>& opts)
    {
        auto expands = GetObjectExpandOpts(opts);
        auto objectOpts = GetExpandOpts(opts);

        if(Contains<std::string>(expands, "topic"))
        {
            std::string id = field.Topic.As<std::string>();
            field.Topic = (co_await TopicQueryType::Get(id, expands))[0];
        }

        info.Result(field);
        co_return;
    }
}
