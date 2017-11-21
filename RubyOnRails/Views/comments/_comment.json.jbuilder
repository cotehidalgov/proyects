json.extract! comment, :id, :comment_text, :comment_date, :created_at, :updated_at
json.url comment_url(comment, format: :json)
