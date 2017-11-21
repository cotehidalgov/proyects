json.extract! proyect, :id, :name, :description, :initial_date, :due_date, :actual_money, :goal_money, :score, :created_at, :updated_at
json.url proyect_url(proyect, format: :json)
